#ifndef SNAPINTERFACE_H
#define SNAPINTERFACE_H

#include <QObject>
#include "pages/itempage.h"
#include "pages/categorypage.h"

class SnapInterface : public QObject
{
    Q_OBJECT

public:
    virtual void itemPageData(ItemPage *page, QString app) = 0;
    virtual void install(ItemPage *page, QString app, bool classic) = 0;
    virtual void uninstall(ItemPage *page, QString app) = 0;
    virtual void search(CategoryPage *parent, QString query) = 0;
    virtual void installed(CategoryPage *parent) = 0;
    virtual void launch(QString app) = 0;
};

Q_DECLARE_INTERFACE(SnapInterface, "org.dekzi.dde-store.SnapInterface")

#endif // SNAPINTERFACE_H