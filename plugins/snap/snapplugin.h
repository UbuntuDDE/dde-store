#ifndef SNAPPLUGIN_H
#define SNAPPLUGIN_H

#include <QObject>
#include <Snapd/Client>
#include <pages/itempage.h>
#include <pages/categorypage.h>
#include <plugins/snap/snapinterface.h>

class SnapPlugin : public SnapInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.dekzi.dde-store.SnapPlugin")
    Q_INTERFACES(SnapInterface)

public:
    SnapPlugin();
    void itemPageData(ItemPage *page, QString app);
    void install(ItemPage *page, QString app, bool classic);
    void uninstall(ItemPage *page, QString app);
    void search(CategoryPage *parent, QString query);
    void installed(CategoryPage *parent);
    void launch(QString app);

private:
    CategoryPage::App categoryPageData(QSnapdSnap *snap);
    QSnapdClient *client;
    int requestClassic();
    void error(QSnapdRequest::QSnapdError err, QString error);
    QStringList installedSnaps;
};

#endif // SNAPPLUGIN_H