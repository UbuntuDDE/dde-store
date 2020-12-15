#ifndef PACKAGEKITHEPLER_H
#define PACKAGEKITHEPLER_H

#include <QObject>
#include "pages/categorypage.h"
#include "pages/itempage.h"
#include "pages/updatespage.h"
#include <Daemon>

using namespace PackageKit;

class PackageKitHelper : public QObject
{
    Q_OBJECT

public:
    static PackageKitHelper *instance();
    void getInstalled(CategoryPage *parent);
    void getUpdates(UpdatesPage *parent, bool refreshCache = false);
    void launch(QString packageId);
    void itemPageData(ItemPage *parent, QString package);
    void install(ItemPage *parent, QString packageId);
    void uninstall(ItemPage *parent, QString packageId);
    void update(UpdatesPage *parent, QStringList updates);
    bool preventClose = false;
    bool updatesAvailable = false;
    QString nameFromID(QString ID);
private:
    static PackageKitHelper *currentInstance;
    void refreshCacheAndGetUpdates(UpdatesPage *parent);
private slots:
    void error(Transaction::Error err, const QString &error);
};

#endif // PACKAGEKITHEPLER_H