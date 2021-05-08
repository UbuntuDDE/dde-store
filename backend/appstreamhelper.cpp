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
        for (const QString &pkgName : app.packageNames()) {
            appList.insert(pkgName, app);
        }
    }

    qDebug() << "[ APPSTREAM ] Loaded metadata pool";
}

AppStreamHelper::appData AppStreamHelper::getAppData(QString package)
{
    appData data;
    if (hasAppData(package)) {
        const AppStream::Component app = appList.value(package);

        data.name = app.name();
        data.developer = app.developerName();
        data.description = app.description();
        data.id = app.id();
        
        for (const AppStream::Screenshot &screenshot : app.screenshots()) {
            const auto images = screenshot.images();
            for (const AppStream::Image &image : images) {
                if (image.kind() == AppStream::Image::KindSource)
                    data.screenshots << image.url();
            }
        }

        for (AppStream::Icon icon : app.icons()) {
            if (icon.kind() == AppStream::Icon::KindStock) {
                data.icon = QIcon::fromTheme(icon.name());
                if (!data.icon.isNull())
                    break;
            }
        }

        if (data.icon.isNull()) {
            AppStream::Icon packageIcon = app.icon(QSize(64, 64));
            data.icon.addFile(packageIcon.url().toLocalFile(), packageIcon.size());
        }
    }

    if (data.name.isNull())
        data.name = package;

    if (data.icon.isNull())
        data.icon = QIcon::fromTheme("application-x-executable");

    return data;
}

QStringList AppStreamHelper::category(QString category)
{
    QStringList pkgNames;
    for (const QString &key : appList.keys()) {
        if (pkgNames.length() >= settings::instance()->maxItems())
            break;
        if (appList.value(key).categories().contains(category))
            pkgNames << key;
    }
    return pkgNames;
}

QStringList AppStreamHelper::search(QString query)
{
    QStringList pkgNames;
    for (const QString &key : appList.keys()) {
        if (pkgNames.length() >= settings::instance()->maxItems())
            break;
        
        AppStream::Component component = appList.value(key);
        bool noMatch = false;
        for (const QString &item : query.split(QRegExp("\\s"), Qt::SkipEmptyParts)) {
            if (!component.name().contains(item, Qt::CaseInsensitive) && !component.description().contains(item, Qt::CaseInsensitive))
                noMatch = true;
        }
        if (!noMatch)
            pkgNames << key;
    }
    return pkgNames;
}

bool AppStreamHelper::hasAppData(QString package)
{
    return appList.contains(package);
}

QString AppStreamHelper::packageFromID(QString ID)
{
    for (const QString &key : appList.keys()) {
        if (appList.value(key).id() == ID)
            return key;
    }
    return QString();
}

QString AppStreamHelper::IDFromPackage(QString package)
{
    return getAppData(package).id;
}