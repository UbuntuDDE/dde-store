#include "snapplugin.h"
#include "backend/packagekithelper.h"
#include "backend/ratingshelper.h"
#include "backend/settings.h"
#include <QLocale>
#include <QBuffer>
#include <QImageReader>
#include <QProcess>
#include <QDebug>
#include <DNotifySender>
#include <DDialog>
#include <DStyle>

DWIDGET_USE_NAMESPACE

SnapPlugin::SnapPlugin()
{
    client = new QSnapdClient;
}

void SnapPlugin::itemPageData(ItemPage *page, QString app)
{
    QSnapdSnap *snap;
    if (installedSnaps.contains(app)) {
        auto request = client->getSnap(app);
        request->runSync();
        snap = request->snap();
    } else {
        auto request = client->find(QSnapdClient::MatchName, app);
        request->runSync();
        snap = request->snap(0);
    }

    AppStreamHelper::appData data;
    data.name = snap->title();
    data.description = snap->description();
    data.summary = snap->summary();
    data.developer = snap->publisherDisplayName();
    QList<QUrl> screenshots;
    for (int i = 0; i < snap->mediaCount(); i++) {
        if (snap->media(i)->type() == "screenshot") {
            screenshots << QUrl(snap->media(i)->url());
        }
    }
    data.screenshots = screenshots;
    if (snap->commonIds().length() > 0) {
        data.id = snap->commonIds()[0];
    } else {
        data.id = QString("io.snapcraft.%1-%2").arg(snap->name()).arg(snap->id());
    }

    if (snap->icon().startsWith("/")) {
        data.icon = QIcon(snap->icon());
    } else if (snap->icon().isEmpty()) {
        data.icon = QIcon::fromTheme("application-x-executable");
    } else {
        auto iconrequest = client->getIcon(app);
        iconrequest->runSync();
        if (!iconrequest->error()) {
            QBuffer buffer;
            buffer.setData(iconrequest->icon()->data());
            QImageReader reader(&buffer);
            reader.read();
            data.icon = QIcon(QPixmap::fromImage(reader.read()));
        }
    }

    if (data.icon.isNull()) {
        data.icon = QIcon::fromTheme("application-x-executable");
    }

    page->setData(data);

    if (installedSnaps.contains(snap->name())) {
        if (snap->snapType() == QSnapdEnums::SnapTypeApp) {
            page->setInstallButton(app, ItemPage::Launchable);
        } else {
            page->setInstallButton(app, ItemPage::Installed);
        }
    } else {
        page->setInstallButton(app, ItemPage::NotInstalled, QLocale().formattedDataSize(snap->downloadSize()));
    }
}

void SnapPlugin::install(ItemPage *page, QString app, bool classic)
{
    QSnapdInstallRequest *request;
    if (classic) {
        request = client->install(QSnapdClient::Classic, app);
    } else {
        request = client->install(app);
    }
    PackageKitHelper::instance()->preventClose = true;
    request->runAsync();
    qDebug() << "[ INSTALL ] Installing snap" << app;
    connect(request, &QSnapdRequest::progress, this, [ = ] {
        int percent = 0;
        for (int i = 0; i < request->change()->taskCount(); i++) {
            percent += (100 * request->change()->task(i)->progressDone()) / request->change()->task(i)->progressTotal();
        }
        percent /= qMax(request->change()->taskCount(), 1);
        qDebug() << "[ INSTALL ]" << QString::number(percent) + "%";
        page->setInstallButton(app, ItemPage::Installing, QString::number(percent));
    });
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        PackageKitHelper::instance()->preventClose = false;
        switch (request->error())
        {
        case QSnapdRequest::NoError:
            if (settings::instance()->notifyInstall()) {
                DUtil::DNotifySender(tr("Installed \"%1\"").arg(app)).appIcon("dialog-ok").appName("DDE Store").timeOut(5000).call();
            }
            page->setInstallButton(app, ItemPage::Installed);
            installedSnaps << app;
            qDebug() << "[ INSTALL ] Snap installed";
            break;
        case QSnapdRequest::NeedsClassic:
            if (requestClassic() == 1) {
                install(page, app, true);
            }
            break;
        default:
            page->setInstallButton(app, ItemPage::NotInstalled);
            error(request->error(), request->errorString());
            break;
        }
    });
}

void SnapPlugin::uninstall(ItemPage *page, QString app)
{
    auto request = client->remove(app);
    PackageKitHelper::instance()->preventClose = true;
    request->runAsync();
    qDebug() << "[ UNINSTALL ] Uninstalling snap" << app;
    connect(request, &QSnapdRequest::progress, this, [ = ] {
        int percent = 0;
        for (int i = 0; i < request->change()->taskCount(); i++) {
            percent += (100 * request->change()->task(i)->progressDone()) / request->change()->task(i)->progressTotal();
        }
        percent /= qMax(request->change()->taskCount(), 1);
        qDebug() << "[ UNINSTALL ]" << QString::number(percent) + "%";
        page->setInstallButton(app, ItemPage::Installing, QString::number(percent));
    });
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        if (request->error() != QSnapdRequest::NoError) {
            error(request->error(), request->errorString());
        } else {
            if (settings::instance()->notifyInstall()) {
                DUtil::DNotifySender(tr("Uninstalled \"%1\"").arg(app)).appIcon("dialog-ok").appName("DDE Store").timeOut(5000).call();
            }
            installedSnaps.removeAll(app);
            page->setInstallButton(app, ItemPage::NotInstalled);
            qDebug() << "[ Uninstalled ] Snap uninstalled";
        }
        PackageKitHelper::instance()->preventClose = false;
    });
}

CategoryPage::App SnapPlugin::categoryPageData(QSnapdSnap *snap)
{
    CategoryPage::App data;
    data.name = snap->title();
    if (snap->icon().startsWith("/")) {
        data.icon = QIcon(snap->icon());
    } else if (snap->icon().isEmpty()) {
        data.icon = QIcon::fromTheme("application-x-executable");
    } else {
        auto iconrequest = client->getIcon(snap->name());
        iconrequest->runSync();
        if (!iconrequest->error()) {
            QBuffer buffer;
            buffer.setData(iconrequest->icon()->data());
            QImageReader reader(&buffer);
            reader.read();
            data.icon = QIcon(QPixmap::fromImage(reader.read()));
        }
    }
    if (data.icon.isNull()) {
        data.icon = QIcon::fromTheme("application-x-executable");
    }
    data.package = snap->name();
    if (snap->commonIds().length() > 0) {
        data.id = snap->commonIds()[0];
    } else {
        data.id = QString("io.snapcraft.%1-%2").arg(snap->name()).arg(snap->id());
    }
    data.ratings = RatingsHelper::instance()->totalRatings(data.id);
    data.source = CategoryPage::Backend::Snap;
    return data;
}

void SnapPlugin::search(CategoryPage *parent, QString query)
{
    auto request = client->find(query);
    request->runAsync();
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        for (int i = 0; i < request->snapCount(); i++) {
            parent->insertItem(categoryPageData(request->snap(i)));
        }
        parent->load();
    });
}

void SnapPlugin::installed(CategoryPage *parent)
{
    auto request = client->getSnaps();
    request->runAsync();
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        for (int i = 0; i < request->snapCount(); i++) {
            if (request->snap(i)->snapType() == QSnapdEnums::SnapTypeApp) {
                if (!installedSnaps.contains(request->snap(i)->name())) {
                    installedSnaps << request->snap(i)->name();
                }
                parent->insertItem(categoryPageData(request->snap(i)));
            }
        }
        parent->load();
    });
}

void SnapPlugin::launch(QString app)
{
    QProcess::startDetached("snap", {"run", app});
}

int SnapPlugin::requestClassic()
{
    DDialog dialog(tr("Classic mode required"), tr("This snap requires confinement to be disabled via classic mode"));
    dialog.setIcon(DStyle().standardIcon(QStyle::SP_MessageBoxWarning));
    dialog.addButton(tr("Cancel"));
    dialog.addButton(tr("Continue"), true, DDialog::ButtonRecommend);
    return dialog.exec();
}

void SnapPlugin::error(QSnapdRequest::QSnapdError err, QString error)
{
    QString errorText = tr("Snap Error");
    qDebug() << "[ ERROR ]" << errorText << err << error;
    DDialog dialog(errorText, QString(QMetaEnum::fromType<QSnapdRequest::QSnapdError>().valueToKey(err)) + "<br>" + error);
    dialog.setIcon(DStyle().standardIcon(QStyle::SP_MessageBoxCritical));
    dialog.addButton("OK");
    dialog.exec();
}