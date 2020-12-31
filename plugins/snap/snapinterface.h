#ifndef SNAPINTERFACE_H
#define SNAPINTERFACE_H

#include <QObject>
#include "pages/itempage.h"
#include "pages/categorypage.h"

class SnapInterface : public QObject
{
    Q_OBJECT

public:
    void itemPageData(ItemPage *page, QString app);
    void install(ItemPage *page, QString app, bool classic);
    void uninstall(ItemPage *page, QString app);
    void search(CategoryPage *parent, QString query);
    void installed(CategoryPage *parent);
    static void launch(QString app);

private:
    int requestClassic();
    QStringList installedSnaps;
};

#endif // SNAPINTERFACE_H