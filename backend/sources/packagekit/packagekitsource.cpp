#include "packagekitsource.h"
#include "backend/settings.h"
#include "backend/ratingshelper.h"
#include <Details>
#include <AppStreamQt5/pool.h>
#include <AppStreamQt5/icon.h>
#include <AppStreamQt5/screenshot.h>
#include <AppStreamQt5/image.h>
#include <AppStreamQt5/developer.h>

using namespace PackageKit;
using namespace AppStream;

PackageKitSource::PackageKitSource()
{
    AppStream::Pool pool;
    if (!pool.load())
        Source::error(tr("ERROR: Unable to open AppStream metadata pool") + " - " + pool.lastError());

    for (const Component & app : pool.componentsByKind(AppStream::Component::KindDesktopApp)) {
        for (const QString & pkgName : app.packageNames()) {
            metadata.insert(pkgName, app);
        }
    }

    connect(RatingsHelper::instance(), &RatingsHelper::fetched, this, [ = ] {
        for (App *app : ratingsQueue)
            app->ratings = RatingsHelper::instance()->totalRatings(app->id);
        ratingsQueue.clear();
    });
    
    Daemon::global()->setHints(QStringList() << QStringLiteral("interactive=true"));
}

QString PackageKitSource::name()
{
    return "PackageKit";
}

void PackageKitSource::getInstalled()
{
    Transaction *transaction = Daemon::getPackages(Transaction::FilterInstalled | Transaction::FilterApplication);
    QList<App*> *installed = new QList<App*>();

    connect(transaction, &Transaction::errorCode, this, &PackageKitSource::error);
    
    connect(transaction, &Transaction::package, this, [ = ] (Transaction::Info info, const QString &packageID) {
        if (metadata.contains(Daemon::packageName(packageID)))
            installed->append(getData(Daemon::packageName(packageID), true));
    });

    connect(transaction, &Transaction::finished, this, [ = ] {
        if (RatingsHelper::instance()->available)
            emit(gotInstalled(*installed));
        else
            connect(RatingsHelper::instance(), &RatingsHelper::fetched, this, [ = ] { emit(gotInstalled(*installed)); });
    });
}

void PackageKitSource::getUpdates(bool refreshCache)
{
    if (refreshCache) {
        connect(Daemon::refreshCache(false), &Transaction::finished, this, &Source::getUpdates);
        return;
    }

    Transaction *transaction = Daemon::getUpdates();
    updates.clear();

    connect(transaction, &Transaction::errorCode, this, &PackageKitSource::error);

    connect(transaction, &Transaction::package, this, [ = ] (Transaction::Info info, const QString &packageID) {
        updates.insert(packageID, getData(Daemon::packageName(packageID)));
    });

    connect(transaction, &Transaction::finished, this, [ = ] {
        Transaction *getDetails = Daemon::getDetails(updates.keys());
        connect(getDetails, &Transaction::details, this, [ = ] (const Details &values) {
            updates[values.packageId()]->downloadSize = values.size();
        });
        connect(getDetails, &Transaction::finished, this, [ = ] {
            emit(gotUpdates(updates.values()));
        });
    });
}

void PackageKitSource::getCategory(QString category)
{
    QList<App*> items;
    for (QString key : metadata.keys()) {
        if (items.length() >= settings::instance()->maxItems())
            break;
        if (metadata.value(key).categories().contains(category))
            items << getData(key, true);
    }
    if (RatingsHelper::instance()->available)
        emit(gotCategory(category, items));
    else
        connect(RatingsHelper::instance(), &RatingsHelper::fetched, this, [ = ] { emit(gotCategory(category, items)); });
}

void PackageKitSource::getFullData(App *app)
{
    if (metadata.contains(app->package)) {
        const Component data = metadata.value(app->package);

        app->developer = data.developer().name();
        app->description = data.description();
        
        for (Screenshot screenshot : data.screenshotsAll()) {
            for (Image image : screenshot.images()) {
                if (image.kind() == Image::KindSource)
                    app->screenshots << image.url();
            }
        }

        if (app->ratings == 0)
            app->ratings = RatingsHelper::instance()->totalRatings(app->id);
    }

    Transaction *resolve = Daemon::resolve(app->package);
    connect(resolve, &Transaction::package, this, [ = ] (Transaction::Info info, const QString &packageID) {
        app->state = info == Transaction::InfoInstalled ? App::Installed : App::NotInstalled;
        Transaction *getDetails = Daemon::getDetails(packageID);
        connect(getDetails, &Transaction::details, this, [ = ] (const Details &values) {
            app->downloadSize = values.size();
        });
        connect(getDetails, &Transaction::finished, this, [ = ] {
            app->fullData = true;
            emit(gotFullData(app));
        });
    });
}

void PackageKitSource::install(App *app)
{
    preventingClose = true;
    Transaction *getID = Daemon::resolve(app->package);
    connect(getID, &Transaction::package, this, [ = ] (Transaction::Info info, const QString &packageID) {
        Transaction *transaction = Daemon::installPackage(packageID);
        app->state = App::Installing;
        emit(stateChanged(app));

        connect(transaction, &Transaction::errorCode, this, &PackageKitSource::error);

        connect(transaction, &Transaction::percentageChanged, this, [ = ] {
            if (transaction->percentage() <= 100)
                emit(percentageChanged(app, transaction->percentage()));
        });

        connect(transaction, &Transaction::finished, this, [ = ] {
            app->state = App::Installed;
            emit(stateChanged(app));
            emit(installFinished(app));
            preventingClose = false;
        });
    });
}

void PackageKitSource::uninstall(App *app)
{
    preventingClose = true;
    Transaction *getID = Daemon::resolve(app->package);
    connect(getID, &Transaction::package, this, [ = ] (Transaction::Info info, const QString &packageID) {
        Transaction *transaction = Daemon::removePackage(packageID);
        app->state = App::Installing;
        emit(stateChanged(app));

        connect(transaction, &Transaction::errorCode, this, &PackageKitSource::error);

        connect(transaction, &Transaction::percentageChanged, this, [ = ] {
            if (transaction->percentage() <= 100)
                emit(percentageChanged(app, transaction->percentage()));
        });

        connect(transaction, &Transaction::finished, this, [ = ](PackageKit::Transaction::Exit status, uint) {
            if (status == Transaction::ExitSuccess) {
                app->state = App::NotInstalled;
                emit(stateChanged(app));
                emit(uninstallFinished(app));
            } else {
                app->state = App::Installed;
                emit(stateChanged(app));
            }
            preventingClose = false;
        });
    });
}

void PackageKitSource::search(QString query)
{
    QList<App*> results;
    for (QString key : metadata.keys()) {
        if (results.length() >= settings::instance()->maxItems())
            break;
        Component component = metadata.value(key);
        bool noMatch = false;
        for (QString item : query.split(QRegExp("\\s"), Qt::SkipEmptyParts)) {
            if (!component.name().contains(item, Qt::CaseInsensitive) && !component.description().contains(item, Qt::CaseInsensitive))
                noMatch = true;
        }
        if (!noMatch)
            results << getData(key);
    }
    if (RatingsHelper::instance()->available)
        emit(searchFinished(results));
    else
        connect(RatingsHelper::instance(), &RatingsHelper::fetched, this, [ = ] { emit(searchFinished(results)); });
}

void PackageKitSource::launch(App *app)
{
    
}

void PackageKitSource::update()
{ 
    preventingClose = true;
    Transaction *transaction = Daemon::updatePackages(updates.keys());

    connect(transaction, &Transaction::errorCode, this, &PackageKitSource::error);

    connect(transaction, &Transaction::itemProgress, this, [ = ] (const QString &packageID, Transaction::Status, uint percent) {
        if (percent <= 100)
            emit(percentageChanged(updates[packageID], percent));
    });

    connect(transaction, &Transaction::finished, this, [ = ] {
        emit(updateFinished());
        preventingClose = false;
    });
}

QStringList PackageKitSource::urlSchemes()
{
    return { "appstream" };
}

App *PackageKitSource::appFromUrl(QUrl url)
{
    QString id(QUrl::fromLocalFile(url.toString()).fileName());
    QString shortId;
    if (id.endsWith(".desktop"))
        shortId = id.left(id.lastIndexOf("."));

    for (QString key : metadata.keys()) {
        if (metadata[key].id() == id || metadata[key].id() == shortId)
            return getData(key);
    }
    
    return nullptr;
}

void PackageKitSource::error(Transaction::Error code, const QString &text)
{
    Source::error(QVariant::fromValue(code).toString() + " - " + text);
}

App *PackageKitSource::getData(QString package, bool fetchRatings)
{
    App *app = new App();

    if (metadata.contains(package)) {
        const Component data = metadata.value(package);

        app->name = data.name();
        app->id = data.id();
        if (fetchRatings) {
            if (RatingsHelper::instance()->available)
                app->ratings = RatingsHelper::instance()->totalRatings(app->id);
            else
                ratingsQueue << app;
        }

        for (Icon icon : data.icons()) {
            if (icon.kind() == Icon::KindStock) {
                app->icon = QIcon::fromTheme(icon.name());
                if (!app->icon.isNull())
                    break;
            }
        }

        if (app->icon.isNull()) {
            Icon packageIcon = data.icon(QSize(64, 64));
            app->icon.addFile(packageIcon.url().toLocalFile(), packageIcon.size());
        }
    }

    app->package = package;
    app->source = this;
    app->hasMetadata = metadata.contains(package);

    if (app->name.isEmpty())
        app->name = package;
    if (app->icon.isNull())
        app->icon = QIcon::fromTheme("application-x-executable");
    
    return app;
}

App *PackageKitSource::getDataFromID(QString ID)
{
    for (QString key : metadata.keys()) {
        if (metadata.value(key).id() == ID)
            return getData(key);
    }
    return getData(ID);
}
