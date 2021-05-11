#include "snapsource.h"
#include <QUrl>
#include <QProcess>
#include <QBuffer>
#include <QImageReader>
#include <DStyle>
#include <DDialog>

SnapSource::SnapSource()
    : client(new QSnapdClient(this))
{

}

QString SnapSource::name()
{
    return "Snap";
}

void SnapSource::getInstalled()
{
    auto request = client->getSnaps();
    request->runAsync();
    QList<App*> *apps = new QList<App*>;
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        installed.clear();
        for (int i = 0; i < request->snapCount(); i++) {
            apps->append(getData(request->snap(i)));
            installed << request->snap(i)->id();
        }
        emit(gotInstalled(*apps));
    });
}

void SnapSource::getUpdates(bool refreshCache)
{
    emit(gotUpdates(QList<App*>()));
}

void SnapSource::getFullData(App *app)
{
    auto request = client->find(QSnapdClient::MatchName, app->package);
    request->runAsync();

    connect(request, &QSnapdRequest::complete, this, [ = ] {
        QSnapdSnap *snap = request->snap(0);
        
        app->developer = snap->publisherDisplayName();
        app->description = snap->description();
        app->downloadSize = snap->downloadSize();
        app->state = installed.contains(app->id) ? (app->hasMetadata ? App::Launchable : App::Installed) : App::NotInstalled;

        for (int i = 0; i < snap->mediaCount(); i++) {
            if (snap->media(i)->type() == "screenshot")
                app->screenshots << QUrl(snap->media(i)->url());
        }

        app->fullData = true;
        emit(gotFullData(app));
    });
}

void SnapSource::install(App *app)
{
    QSnapdInstallRequest *request;
    if (useClassic) {
        request = client->install(QSnapdClient::Classic, app->package);
        useClassic = false;
    } else {
        request = client->install(app->package);
    }
    request->runAsync();
    preventingClose = true;
    app->state = App::Installing;
    emit(stateChanged(app));
    connect(request, &QSnapdRequest::progress, this, [ = ] {
        int percent = 0;
        for (int i = 0; i < request->change()->taskCount(); i++) {
            percent += (100 * request->change()->task(i)->progressDone()) / request->change()->task(i)->progressTotal();
        }
        percent /= qMax(request->change()->taskCount(), 1);
        emit(percentageChanged(app, percent));
    });
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        if (request->error() == QSnapdRequest::NoError) {
            app->state = App::Installed;
            emit(stateChanged(app));
            installed << app->id;
        } else if (request->error() == QSnapdRequest::NeedsClassic) {
            if (requestClassic() == 1) {
                useClassic = true;
                install(app);
            }
        } else {
            error(QVariant::fromValue(request->error()).toString() + " - " + request->errorString());
            app->state = App::NotInstalled;
            emit(stateChanged(app));
        }
        emit(installFinished(app));
        preventingClose = false;
    });
}

void SnapSource::uninstall(App *app)
{
    auto request = client->remove(app->package);
    request->runAsync();
    preventingClose = true;
    app->state = App::Installing;
    emit(stateChanged(app));
    connect(request, &QSnapdRequest::progress, this, [ = ] {
        int percent = 0;
        for (int i = 0; i < request->change()->taskCount(); i++) {
            percent += (100 * request->change()->task(i)->progressDone()) / request->change()->task(i)->progressTotal();
        }
        percent /= qMax(request->change()->taskCount(), 1);
        emit(percentageChanged(app, percent));
    });
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        if (request->error() == QSnapdRequest::NoError) {
            app->state = App::NotInstalled;
            emit(stateChanged(app));
            installed.removeAll(app->id);
        } else {
            error(QVariant::fromValue(request->error()).toString() + " - " + request->errorString());
            app->state = App::Installed;
            emit(stateChanged(app));
        }
        emit(uninstallFinished(app));
        preventingClose = false;
    });
}

void SnapSource::launch(App *app)
{
    QProcess::startDetached("snap", { "run", app->package });
}

void SnapSource::search(QString query)
{
    auto request = client->find(query);
    request->runAsync();
    QList<App*> *results = new QList<App*>;
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        for (int i = 0; i < request->snapCount(); i++) {
            results->append(getData(request->snap(i)));
        }
        emit(searchFinished(*results));
    });
}

void SnapSource::update()
{

}

QStringList SnapSource::urlSchemes()
{
    return { "snap" };
}

App *SnapSource::appFromUrl(QUrl url)
{
    auto request = client->find(QSnapdClient::MatchCommonId, QUrl::fromLocalFile(url.toString()).fileName());
    request->runSync();
    return getData(request->snap(0));
}

App *SnapSource::getData(QSnapdSnap *snap)
{
    App *app = new App();
    app->name = snap->title();
    app->id = snap->id();

    if (snap->icon().startsWith("/")) {
        app->icon = QIcon(QPixmap(snap->icon()));
    } else if (snap->icon().isEmpty()) {
        app->icon = QIcon::fromTheme("application-x-executable");
    } else {
        auto iconrequest = client->getIcon(app->package);
        iconrequest->runSync();
        if (!iconrequest->error()) {
            QBuffer buffer;
            buffer.setData(iconrequest->icon()->data());
            QImageReader reader(&buffer);
            reader.read();
            app->icon = QIcon(QPixmap::fromImage(reader.read()));
        }
    }
    if (app->icon.isNull())
        app->icon = QIcon::fromTheme("application-x-executable");
    
    app->package = snap->name();
    app->source = this;
    app->hasMetadata = snap->snapType() == QSnapdEnums::SnapTypeApp;

    return app;
}

int SnapSource::requestClassic()
{
    Dtk::Widget::DDialog dialog(tr("Classic mode required"), tr("This snap requires confinement to be disabled via classic mode"));
    dialog.setIcon(Dtk::Widget::DStyle().standardIcon(QStyle::SP_MessageBoxWarning));
    dialog.addButton(tr("Cancel"));
    dialog.addButton(tr("Continue"), true, Dtk::Widget::DDialog::ButtonRecommend);
    return dialog.exec();
}
