#include "backend/packagekithelper.h"
#include "backend/appstreamhelper.h"
#include "backend/ratingshelper.h"
#include "backend/settings.h"
#include <Details>
#include <QLocale>
#include <QStandardPaths>
#include <QProcess>
#include <QDebug>
#include <DNotifySender>
#include <DDialog>
#include <DStyle>
#ifdef SNAP
#include "backend/snaphelper.h"
#endif

DWIDGET_USE_NAMESPACE

PackageKitHelper *PackageKitHelper::currentInstance = nullptr;

PackageKitHelper *PackageKitHelper::instance()
{
    if (!currentInstance) {
        currentInstance = new PackageKitHelper;
    }
    return currentInstance;
}

void PackageKitHelper::getInstalled(CategoryPage *parent)
{
    Transaction *transaction = Daemon::getPackages(Transaction::FilterInstalled | Transaction::FilterApplication);

    connect(transaction, &Transaction::errorCode, this, &PackageKitHelper::error);

    qDebug() << "[ INSTALLED ] Getting installed applications...";

    connect(transaction, &Transaction::package, this, [ = ] (Transaction::Info, const QString &packageId) {
        auto data = AppStreamHelper::instance()->getAppData(Transaction::packageName(packageId));
        CategoryPage::App app;
        app.name = data.name;
        app.icon = data.icon;
        app.id = Transaction::packageName(packageId);
        app.ratings = RatingsHelper::instance()->totalRatings(data.id);
        app.source = CategoryPage::Backend::PackageKit;
        parent->insertItem(app);
        qDebug() << "[ INSTALLED ] Found installed app" << Transaction::packageName(packageId);
    });

    connect(transaction, &Transaction::finished, this, [ = ] {
#ifdef SNAP
        SnapHelper::instance()->installed(parent);
#else
        parent->load();
#endif
        qDebug() << "[ INSTALLED ] Retrieved installed apps";
    });
}

void PackageKitHelper::getUpdates(UpdatesPage *parent, bool refreshCache)
{
    if (refreshCache) {
        refreshCacheAndGetUpdates(parent);
        return;
    }
    
    QStringList *pkgList = new QStringList;
    Transaction *getupdates = Daemon::getUpdates();
    qDebug() << "[ UPDATES ] Checking for updates...";
    connect(getupdates, &Transaction::package, this, [ = ] (Transaction::Info, const QString &packageId) {
        pkgList->append(packageId);
        qDebug() << "[ UPDATES ] Update found for app" << Transaction::packageName(packageId);
    });
    connect(getupdates, &Transaction::errorCode, this, &PackageKitHelper::error);
    connect(getupdates, &Transaction::finished, this, [ = ] {
        QHash<QString, int> *apps = new QHash<QString, int>;
        if (pkgList->length() > 0) {
            Transaction *getdetails = Daemon::getDetails(*pkgList);
            connect(getdetails, &Transaction::details, this, [ = ] (const Details &details) {
                apps->insert(details.packageId(), details.size());
            });
            connect(getdetails, &Transaction::errorCode, this, &PackageKitHelper::error);
            connect(getdetails, &Transaction::finished, this, [ = ] {
                if (settings::instance()->notifyAvailableUpdates()) {
                    Dtk::Core::DUtil::DNotifySender(tr("Updates Available")).appIcon("system-updated").appName("DDE Store").timeOut(10000).call();
                }
                parent->loadData(*apps);
                qDebug() << "[ UPDATES ] Updates found:" << apps->size();
                updatesAvailable = true;
            });
        } else {
            qDebug() << "[ UPDATES ] System is up to date";
            parent->loadData(*apps);
            updatesAvailable = false;
        }
    });
}

void PackageKitHelper::launch(QString packageId)
{
    AppStreamHelper::appData AppStreamData = AppStreamHelper::instance()->getAppData(Transaction::packageName(packageId));
    QStringList paths = QStandardPaths::locateAll(QStandardPaths::ApplicationsLocation, AppStreamData.id);
    int first = paths[0].lastIndexOf("/");
    int last = paths[0].lastIndexOf(".desktop");
    QProcess::startDetached("gtk-launch", QStringList() << paths[0].mid(first + 1, last - first - 1));
}

void PackageKitHelper::itemPageData(ItemPage *parent, QString package)
{
    Transaction *resolve = Daemon::resolve(package);
    connect(resolve, &Transaction::errorCode, this, &PackageKitHelper::error);
    connect(resolve, &Transaction::package, this, [ = ] (Transaction::Info info, const QString &packageId) {
        if (info == Transaction::InfoAvailable) {
            Transaction *getdetails = Daemon::getDetails(packageId);
            connect(getdetails, &Transaction::details, this, [ = ] (const Details &details) {
                QLocale locale;
                parent->setInstallButton(packageId, ItemPage::NotInstalled, locale.formattedDataSize(details.size()));
            });
            connect(getdetails, &Transaction::errorCode, this, &PackageKitHelper::error);
        } else if (info == Transaction::InfoInstalled) {
            AppStreamHelper::appData AppStreamData = AppStreamHelper::instance()->getAppData(Transaction::packageName(packageId));
            QStringList paths = QStandardPaths::locateAll(QStandardPaths::ApplicationsLocation, AppStreamData.id);
            if (paths.length() > 0) {
                parent->setInstallButton(packageId, ItemPage::Launchable);
            } else {
                parent->setInstallButton(packageId, ItemPage::Installed);
            }
        }
    });
}

void PackageKitHelper::install(ItemPage *parent, QString packageId)
{
    Transaction *transaction = Daemon::installPackage(packageId);
    preventClose = true;
    qDebug() << "[ INSTALL ] Installing app" << Transaction::packageName(packageId);
    connect(transaction, &Transaction::percentageChanged, this, [ = ] {
        if (transaction->percentage() <= 100) {
            qDebug() << "[ INSTALL ]" << QString::number(transaction->percentage()) + "%";
            parent->setInstallButton(packageId, ItemPage::Installing, QString::number(transaction->percentage()));
        }
    });
    connect(transaction, &Transaction::errorCode, this, &PackageKitHelper::error);
    connect(transaction, &Transaction::finished, this, [ = ] {
        if (settings::instance()->notifyInstall()) {
            AppStreamHelper::appData data = AppStreamHelper::instance()->getAppData(Transaction::packageName(packageId));
            Dtk::Core::DUtil::DNotifySender(tr("Installed \"%1\"").arg(data.name)).appIcon("dialog-ok").appName("DDE Store").timeOut(5000).call();
        }
        preventClose = false;
        itemPageData(parent, Transaction::packageName(packageId));
        qDebug() << "[ INSTALL ] App installed";
    });
}

void PackageKitHelper::uninstall(ItemPage *parent, QString packageId)
{
    Transaction *transaction = Daemon::removePackage(packageId);
    preventClose = true;
    qDebug() << "[ UNINSTALL ] Uninstalling app" << Transaction::packageName(packageId);
    connect(transaction, &Transaction::percentageChanged, this, [ = ] {
        if (transaction->percentage() <= 100) {
            qDebug() << "[ UNINSTALL ]" << QString::number(transaction->percentage()) + "%";
            parent->setInstallButton(packageId, ItemPage::Installing, QString::number(transaction->percentage()));
        }
    });
    connect(transaction, &Transaction::errorCode, this, &PackageKitHelper::error);
    connect(transaction, &Transaction::finished, this, [ = ] {
        if (settings::instance()->notifyUninstall()) {
            AppStreamHelper::appData data = AppStreamHelper::instance()->getAppData(Transaction::packageName(packageId));
            Dtk::Core::DUtil::DNotifySender(tr("Uninstalled \"%1\"").arg(data.name)).appIcon("dialog-ok").appName("DDE Store").timeOut(5000).call();
        }
        preventClose = false;
        itemPageData(parent, Transaction::packageName(packageId));
        qDebug() << "[ UNINSTALL ] App uninstalled";
    });
}

void PackageKitHelper::update(UpdatesPage *parent, QStringList updates)
{
    Transaction *update = Daemon::updatePackages(updates);
    preventClose = true;
    qDebug() << "[ UPDATES ] Updating apps";
    connect(update, &Transaction::itemProgress, this, [ = ] (const QString &packageId, Transaction::Status, uint percent) {
        if (percent <= 100) {
            qDebug() << "[ UPDATES ]" << Transaction::packageName(packageId) << QString::number(percent) + "%";
            parent->updatePercent(Transaction::packageName(packageId), percent);
        }
    });
    connect(update, &Transaction::errorCode, this, &PackageKitHelper::error);
    connect(update, &Transaction::finished, this, [ = ] {
        getUpdates(parent);
        if (settings::instance()->notifyFinishedUpdates()) {
            Dtk::Core::DUtil::DNotifySender(tr("Updates Installed")).appIcon("system-updated").appName("DDE Store").timeOut(5000).call();
        }
        preventClose = false;
        qDebug() << "[ UPDATES ] Updates complete";
    });
}

void PackageKitHelper::error(Transaction::Error err, const QString &error)
{
    QString errorText = tr("PackageKit Error");
    qDebug() << "[ ERROR ]" << errorText << err << error;
    DDialog dialog(errorText, QString(QMetaEnum::fromType<Transaction::Error>().valueToKey(err)) + "<br>" + error);
    dialog.setIcon(DStyle().standardIcon(QStyle::SP_MessageBoxCritical));
    dialog.addButton("OK");
    dialog.exec();
}

QString PackageKitHelper::nameFromID(QString ID)
{
    return Transaction::packageName(ID);
}

void PackageKitHelper::refreshCacheAndGetUpdates(UpdatesPage *parent)
{
    qDebug() << "[ UPDATES ] Refreshing package cache...";
    Transaction *refreshcache = Daemon::refreshCache(false);
    connect(refreshcache, &Transaction::errorCode, this, &PackageKitHelper::error);
    connect(refreshcache, &Transaction::finished, this, [ = ] {
        qDebug() << "[ UPDATES ] Package cache updated";
        getUpdates(parent);
    });
}
