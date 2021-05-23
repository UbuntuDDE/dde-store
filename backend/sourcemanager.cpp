#include "sourcemanager.h"
#include "sources/packagekit/packagekitsource.h"
#include <QPluginLoader>
#include <DDialog>
#include <DStyle>

DWIDGET_USE_NAMESPACE

SourceManager *SourceManager::currentInstance = nullptr;

SourceManager *SourceManager::instance()
{
    if (!currentInstance)
        currentInstance = new SourceManager;
    return currentInstance;
}

SourceManager::SourceManager()
{
    m_sources << new PackageKitSource;

    QPluginLoader *snapLoader = new QPluginLoader("dde-store/snapplugin");
    if (snapLoader->load())
        m_sources << qobject_cast<Source*>(snapLoader->instance());
    
    QPluginLoader *flatpakLoader = new QPluginLoader("dde-store/flatpakplugin");
    if (flatpakLoader->load())
        m_sources << qobject_cast<Source*>(flatpakLoader->instance());
}

QList<Source*> SourceManager::sources()
{
    return m_sources;
}

Source *SourceManager::getSource(QString name)
{
    for (Source *source : m_sources) {
        if (source->name() == name)
            return source;
    }
    return nullptr;
}

bool SourceManager::preventingClose()
{
    for (Source *source : m_sources) {
        if (source->preventingClose)
            return true;
    }
    return false;
}

void Source::error(QString text)
{
    qDebug() << "[ ERROR ]" << name() + " - " + text;
    DDialog dialog(name() + " Error", text);
    dialog.setIcon(DStyle().standardIcon(QStyle::SP_MessageBoxCritical));
    dialog.addButton("OK");
    dialog.exec();
}
