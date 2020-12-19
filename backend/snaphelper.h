#ifndef SNAPHELPER_H
#define SNAPHELPER_H

#include <QObject>
#include <Snapd/Client>
#include "pages/itempage.h"

class SnapHelper : public QObject
{
    Q_OBJECT

public:
    static SnapHelper *instance();
    void itemPageData(ItemPage *page, QString app);

private:
    static SnapHelper *currentInstance;
    SnapHelper();
    QSnapdClient *client;
};

#endif // SNAPHELPER_H