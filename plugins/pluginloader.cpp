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
    QPluginLoader *loader = new QPluginLoader("dde-store/");
    SnapInterface *interface = qobject_cast<SnapInterface*>(loader->instance());
    snapPlugin = interface;
}