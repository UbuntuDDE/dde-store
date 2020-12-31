#include "plugins/pluginloader.h"
#include <QPluginLoader>

PluginLoader *PluginLoader::currentInstance = nullptr;

PluginLoader *PluginLoader::instance()
{
    if (!currentInstance) {
        currentInstance = new PluginLoader;
    }
    return currentInstance;
}

PluginLoader::PluginLoader()
{
    QPluginLoader *snapLoader = new QPluginLoader("dde-store/snapplugin", this);
    snapPlugin = qobject_cast<SnapInterface*>(snapLoader->instance());

    QPluginLoader *flatpakLoader = new QPluginLoader("dde-store/flatpakplugin", this);
    flatpakPlugin = qobject_cast<FlatpakInterface*>(flatpakLoader->instance());
}