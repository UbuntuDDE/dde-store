#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include "plugins/snap/snapinterface.h"

class PluginLoader : public QObject
{
    Q_OBJECT

public:
    static PluginLoader *instance();
    SnapInterface *snapPlugin;

private:
    PluginLoader();
    static PluginLoader *currentInstance;

};

#endif // PLUGINLOADER_H