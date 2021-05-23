#ifndef FLATPAKSOURCE_H
#define FLATPAKSOURCE_H

#include "../../sourcemanager.h"

class FlatpakSource : public Source
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.dekzi.dde-store.FlatpakPlugin")
    Q_INTERFACES(Source)

public:
    FlatpakSource();
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

};


#endif // FLATPAKSOURCE_H