#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include "plugins/snap/snapinterface.h"
#include "plugins/flatpak/flatpakinterface.h"

class PluginLoader : public QObject
{
    Q_OBJECT

public:
    static PluginLoader *instance();
    SnapInterface *snapPlugin;
    FlatpakInterface *flatpakPlugin;

private:
    PluginLoader();
    static PluginLoader *currentInstance;

};

#endif // PLUGINLOADER_H