#ifndef SNAPHELPER_H
#define SNAPHELPER_H

#include <QObject>
#include <Snapd/Client>
#include "pages/itempage.h"
#include "pages/categorypage.h"

class SnapHelper : public QObject
{
    Q_OBJECT

public:
    static SnapHelper *instance();
    void itemPageData(ItemPage *page, QString app);
    void install(ItemPage *page, QString app, bool classic);
    void uninstall(ItemPage *page, QString app);
    void search(CategoryPage *parent, QString query);
    void installed(CategoryPage *parent);
    CategoryPage::App categoryPageData(QSnapdSnap *snap);

private:
    static SnapHelper *currentInstance;
    SnapHelper();
    QSnapdClient *client;
    int requestClassic();
    void error(QSnapdRequest::QSnapdError err, QString error);
};

#endif // SNAPHELPER_H