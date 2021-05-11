#ifndef SNAPSOURCE_H
#define SNAPSOURCE_H

#include "../../sourcemanager.h"
#include <Snapd/Client>

class SnapSource : public Source
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.dekzi.dde-store.SnapPlugin")
    Q_INTERFACES(Source)

public:
    SnapSource();
    QString name() override;
    void getInstalled() override;
    void getUpdates(bool refreshCache) override;
    void getFullData(App *app) override;
    void install(App *app) override;
    void uninstall(App *app) override;
    void launch(App *app) override;
    void search(QString query) override;
    void update() override;
    QStringList urlSchemes() override;
    App *appFromUrl(QUrl url) override;

private:
    App *getData(QSnapdSnap *snap);
    int requestClassic();
    QSnapdClient *client;
    bool useClassic = false;
    QStringList installed;

};

#endif // SNAPSOURCE_H