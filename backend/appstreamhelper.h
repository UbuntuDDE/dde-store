#ifndef APPSTREAMHELPER_H
#define APPSTREAMHELPER_H

#include <QObject>
#include <QIcon>

#include <AppStreamQt/pool.h>
#include <AppStreamQt/component.h>

using namespace AppStream;

class AppStreamHelper : public QObject
{
    Q_OBJECT

public:
    struct appData
    {
        QString name;
        QIcon icon;
        QString description;
        QString summary;
        QString developer;
        QList<QUrl> screenshots;
        QString id;
    };

public:
    static AppStreamHelper *instance();
    appData getAppData(QString package);
    QStringList category(QString category);
    QStringList search(QString query);
    bool hasAppData(QString package);
private:
    static AppStreamHelper *currentInstance;
    AppStreamHelper();
    QHash<QString, AppStream::Component> appList;
};

#endif // APPSTREAMHELPER_H