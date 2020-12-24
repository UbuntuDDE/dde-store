#include "backend/appstreamhelper.h"
#include "backend/settings.h"
#include <QDebug>
#include <DDialog>
#include <DStyle>
#include <AppStreamQt/icon.h>
#include <AppStreamQt/screenshot.h>
#include <AppStreamQt/image.h>

DWIDGET_USE_NAMESPACE

AppStreamHelper *AppStreamHelper::currentInstance = nullptr;

AppStreamHelper *AppStreamHelper::instance()
{
    if (!currentInstance) {
        currentInstance = new AppStreamHelper;
    }
    return currentInstance;
}

AppStreamHelper::AppStreamHelper()
{
    AppStream::Pool *pool = new AppStream::Pool;

    qDebug() << "[ APPSTREAM ] Loading metadata pool";

    QString error;
    if (!pool->load()) {
        QString errorText = tr("ERROR: Unable to open AppStream metadata pool");
        DDialog dialog(errorText, pool->lastError());
        qDebug() << "[ ERROR ]" << errorText << pool->lastError();
        dialog.setIcon(DStyle().standardIcon(QStyle::SP_MessageBoxCritical));
        dialog.addButton(tr("OK"));
        dialog.exec();
    }

    const QList<AppStream::Component> data = pool->componentsByKind(AppStream::Component::KindDesktopApp);

    for (const AppStream::Component &app : data) {
        const QStringList pkgNames = app.packageNames();
        for (const QString &pkgName : pkgNames) {
            appList.insert(pkgName, app);
        }
    }

    qDebug() << "[ APPSTREAM ] Loaded metadata pool";
}

AppStreamHelper::appData AppStreamHelper::getAppData(QString package)
{
    appData data;
    const QList<AppStream::Component> apps = appList.values(package);
    for (const AppStream::Component &app : apps) {
        data.name = app.name();
        data.developer = app.developerName();
        data.description = app.description();
        data.summary = app.summary();
        data.id = app.id();
        
        for (const AppStream::Screenshot &screenshot : app.screenshots()) {
            const auto images = screenshot.images();
            for (const AppStream::Image &image : images) {
                if (image.kind() == AppStream::Image::KindSource) {
                    data.screenshots << image.url();
                }
            }
        }

        AppStream::Icon packageIcon = app.icon(QSize(64, 64));
        if (packageIcon.kind() == AppStream::Icon::KindLocal || packageIcon.kind() == AppStream::Icon::KindCached) {
            data.icon.addFile(packageIcon.url().toLocalFile(), packageIcon.size());
        } else if (packageIcon.kind() == AppStream::Icon::KindStock) {
            data.icon = QIcon::fromTheme(packageIcon.name());
        };
    }

    if (data.name.isNull()) {
        data.name = package;
    }
    if (data.icon.isNull()) {
        data.icon = QIcon::fromTheme("application-x-executable");
    }

    return data;
}

QStringList AppStreamHelper::category(QString category)
{
    QStringList pkgNames;
    for (const QString &key : appList.keys()) {
        if (pkgNames.length() < settings::instance()->maxItems()) {
            AppStream::Component component = appList.value(key);
            if (component.categories().contains(category)) {
                pkgNames << key;
            }
        }
    }
    return pkgNames;
}

QStringList AppStreamHelper::search(QString query)
{
    QStringList pkgNames;
    for (const QString &key : appList.keys()) {
        if (pkgNames.length() < settings::instance()->maxItems()) {
            AppStream::Component component = appList.value(key);
            const QStringList queryList = query.split(QRegExp("\\s"), QString::SkipEmptyParts);
            bool noMatch = false;
            for (const QString &item : queryList) {
                if (!component.name().contains(item, Qt::CaseInsensitive) && !component.description().contains(item, Qt::CaseInsensitive)) {
                    noMatch = true;
                };
            }
            if (!noMatch) {
                pkgNames << key;
            }
        }
    }
    return pkgNames;
}

bool AppStreamHelper::hasAppData(QString package)
{
    const QList<AppStream::Component> apps = appList.values(package);
    if (apps.length() > 0) {
        return true;
    } else {
        return false;
    }
}

QString AppStreamHelper::packageFromID(QString ID)
{
    QString package = QString();
    for (const QString &key : appList.keys()) {
        AppStream::Component component = appList.value(key);
        if (component.id() == ID) {
            package = key;
        }
    }
    return package;
}

QString AppStreamHelper::IDFromPackage(QString package)
{
    return getAppData(package).id;
}