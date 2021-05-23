#ifndef PACKAGEKITSOURCE_H
#define PACKAGEKITSOURCE_H

#include "backend/sourcemanager.h"
#include <Daemon>
#include <AppStreamQt/component.h>

class PackageKitSource : public Source
{
    Q_OBJECT

public:
    PackageKitSource();
    QString name() override;
    void getInstalled() override;
    void getUpdates(bool refreshCache) override;
    void getCategory(QString category);
    App *getData(QString package, bool fetchRatings = false);
    App *getDataFromID(QString ID);
    void getFullData(App *app) override;
    void install(App *app) override;
    void uninstall(App *app) override;
    void launch(App *app) override;
    void search(QString query) override;
    void update() override;
    QStringList urlSchemes() override;
    App *appFromUrl(QUrl url) override;

signals:
    void gotCategory(QString category, QList<App*> items);

private:
    void error(PackageKit::Transaction::Error code, const QString &text);
    QHash<QString, App*> updates;
    QHash<QString, AppStream::Component> metadata;
    QList<App*> ratingsQueue;
};

#endif // PACKAGEKITSOURCE_H