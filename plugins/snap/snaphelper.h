#ifndef SNAPHELPER_H
#define SNAPHELPER_H

#include <QObject>
#include <Snapd/Client>
#include <pages/itempage.h>
#include <pages/categorypage.h>
#include <plugins/snap/snapinterface.h>

class SnapHelper : public SnapInterface
{
    Q_OBJECT

public:
    SnapHelper();
    void itemPageData(ItemPage *page, QString app);
    void install(ItemPage *page, QString app, bool classic);
    void uninstall(ItemPage *page, QString app);
    void search(CategoryPage *parent, QString query);
    void installed(CategoryPage *parent);
    static void launch(QString app);
    CategoryPage::App categoryPageData(QSnapdSnap *snap);

private:
    QSnapdClient *client;
    int requestClassic();
    void error(QSnapdRequest::QSnapdError err, QString error);
    QStringList installedSnaps;
};

#endif // SNAPHELPER_H