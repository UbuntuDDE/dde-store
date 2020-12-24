#include "snaphelper.h"
#include "backend/packagekithelper.h"
#include "backend/ratingshelper.h"
#include "backend/settings.h"
#include <QLocale>
#include <QBuffer>
#include <QImageReader>
#include <QDebug>
#include <DNotifySender>
#include <DDialog>
#include <DStyle>

DWIDGET_USE_NAMESPACE

SnapHelper *SnapHelper::currentInstance = nullptr;

SnapHelper *SnapHelper::instance()
{
    if (!currentInstance) {
        currentInstance = new SnapHelper;
    }
    return currentInstance;
}

SnapHelper::SnapHelper()
{
    client = new QSnapdClient;
}

void SnapHelper::itemPageData(ItemPage *page, QString app)
{
    auto request = client->find(QSnapdClient::MatchName, app);
    request->runAsync();
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        AppStreamHelper::appData data;
        data.name = request->snap(0)->title();
        data.description = request->snap(0)->description();
        data.summary = request->snap(0)->summary();
        data.developer = request->snap(0)->publisherDisplayName();
        QList<QUrl> screenshots;
        for (int i = 0; i < request->snap(0)->mediaCount(); i++) {
            if (request->snap(0)->media(i)->type() == "screenshot") {
                screenshots << QUrl(request->snap(0)->media(i)->url());
            }
        }
        data.screenshots = screenshots;
        if (request->snap(0)->commonIds().length() > 0) {
            data.id = request->snap(0)->commonIds()[0];
        } else {
            data.id = QString("io.snapcraft.%1-%2").arg(request->snap(0)->name()).arg(request->snap(0)->id());
        }

        if (request->snap(0)->icon().startsWith("/")) {
            data.icon = QIcon(request->snap(0)->icon());
        } else if (request->snap(0)->icon().isEmpty()) {
            data.icon = QIcon::fromTheme("application-x-executable");
        } else {
            auto iconrequest = client->getIcon(app);
            iconrequest->runSync();
            if (!iconrequest->error()) {
                QBuffer *buffer;
                buffer->setData(iconrequest->icon()->data());
                QImageReader reader(buffer);
                reader.read();
                data.icon = QIcon(QPixmap::fromImage(reader.read()));
            }
        }

        if (data.icon.isNull()) {
            data.icon = QIcon::fromTheme("application-x-executable");
        }

        page->setData(data);

        switch (request->snap(0)->status()) {
            case QSnapdEnums::SnapStatusAvailable:
                page->setInstallButton(data.id, ItemPage::NotInstalled, QLocale().formattedDataSize(request->snap(0)->downloadSize()));
                break;
            case QSnapdEnums::SnapStatusInstalled:
                page->setInstallButton(data.id, ItemPage::Installed);
                break;
        };
    });
}

void SnapHelper::install(ItemPage *page, QString app, bool classic)
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
            percent += (100 * request->change()->task(i)->progressDone() / request->change()->task(i)->progressTotal());
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
            qDebug() << "[ INSTALL ] Snap installed";
            break;
        case QSnapdRequest::NeedsClassic:
            page->setInstallButton(app, ItemPage::NotInstalled);
            if (requestClassic() == 1) {
                install(page, app, true);
            }
        default:
            page->setInstallButton(app, ItemPage::NotInstalled);
            error(request->error(), request->errorString());
        }
    });
}

void SnapHelper::uninstall(ItemPage *page, QString app)
{
    auto request = client->remove(app);
    PackageKitHelper::instance()->preventClose = true;
    request->runAsync();
    qDebug() << "[ UNINSTALL ] Uninstalling snap" << app;
    connect(request, &QSnapdRequest::progress, this, [ = ] {
        int percent = 0;
        for (int i = 0; i < request->change()->taskCount(); i++) {
            percent += (100 * request->change()->task(i)->progressDone() / request->change()->task(i)->progressTotal());
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
            page->setInstallButton(app, ItemPage::NotInstalled);
            qDebug() << "[ Uninstalled ] Snap uninstalled";
        }
        PackageKitHelper::instance()->preventClose = false;
    });
}

CategoryPage::App SnapHelper::categoryPageData(QSnapdSnap *snap)
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
            QBuffer *buffer;
            buffer->setData(iconrequest->icon()->data());
            QImageReader reader(buffer);
            reader.read();
            data.icon = QIcon(QPixmap::fromImage(reader.read()));
        }
    }
    if (data.icon.isNull()) {
        data.icon = QIcon::fromTheme("application-x-executable");
    }
    data.id = snap->name();
    if (snap->commonIds().length() > 0) {
        data.ratings = RatingsHelper::instance()->totalRatings(snap->commonIds()[0]);
    } else {
        data.ratings = RatingsHelper::instance()->totalRatings(QString("io.snapcraft.%1-%2").arg(snap->name()).arg(snap->id()));
    }
    data.source = CategoryPage::Backend::Snap;
    return data;
}

void SnapHelper::search(CategoryPage *parent, QString query)
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

void SnapHelper::installed(CategoryPage *parent)
{
    auto request = client->getSnaps();
    request->runAsync();
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        for (int i = 0; i < request->snapCount(); i++) {
            parent->insertItem(categoryPageData(request->snap(i)));
        }
        parent->load();
    });
}

void SnapHelper::getUpdates(UpdatesPage *parent)
{
    auto request = client->findRefreshable();
    request->runAsync();
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        for (int i = 0; i < request->snapCount(); i++) {
            auto data = categoryPageData(request->snap(i));
            parent->insertItem(data.name, data.icon, data.id, request->snap(i)->downloadSize());
            qDebug() << "[ UPDATES ] Snap - Update found" << data.name;
        }
        parent->load();
        qDebug() << "[ UPDATES ] Snap - updates fetched";
    });
}

int SnapHelper::requestClassic()
{
    DDialog dialog(tr("Classic mode required"), tr("This snap requires confinement to be disabled via classic mode"));
    dialog.setIcon(DStyle().standardIcon(QStyle::SP_MessageBoxWarning));
    dialog.addButton(tr("Cancel"));
    dialog.addButton(tr("Continue"), true, DDialog::ButtonRecommend);
    return dialog.exec();
}

void SnapHelper::error(QSnapdRequest::QSnapdError err, QString error)
{
    QString errorText = tr("Snap Error");
    qDebug() << "[ ERROR ]" << errorText << err << error;
    DDialog dialog(errorText, QString(QMetaEnum::fromType<QSnapdRequest::QSnapdError>().valueToKey(err)) + "<br>" + error);
    dialog.setIcon(DStyle().standardIcon(QStyle::SP_MessageBoxCritical));
    dialog.addButton("OK");
    dialog.exec();
}