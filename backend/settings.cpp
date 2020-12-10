#include "settings.h"
#include <QStandardPaths>
#include <QApplication>
#include <qsettingbackend.h>
#include <QDebug>
#include <DSettingsOption>
#include <DSettingsGroup>

settings *settings::currentInstance = nullptr;

settings *settings::instance()
{
    if (!currentInstance) {
        currentInstance = new settings;
    }
    return currentInstance;
}

settings::settings() : QObject(qApp)
{
    QString path = QString("%1/%2/%3/config.conf")
        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
        .arg(qApp->organizationName())
        .arg(qApp->applicationName());

    Dtk::Core::QSettingBackend *backend = new QSettingBackend(path);
    appsettings = DSettings::fromJsonFile("://resources/settings.json");
    appsettings->setBackend(backend);
}

int settings::maxItems()
{
    return appsettings->option("basic.view.maxitems")->value().toInt();
}

int settings::nonApps()
{
    return appsettings->option("basic.view.nonapps")->value().toInt();
}

bool settings::notifyInstall()
{
    return appsettings->option("basic.notifications.install")->value().toBool();
}

bool settings::notifyUninstall()
{
    return appsettings->option("basic.notifications.uninstall")->value().toBool();
}

bool settings::notifyAvailableUpdates()
{
    return appsettings->option("basic.notifications.availableupdates")->value().toBool();
}

bool settings::notifyFinishedUpdates()
{
    return appsettings->option("basic.notifications.finishedupdates")->value().toBool();
}

bool settings::tray()
{
    return appsettings->option("basic.behaviour.tray")->value().toBool();
}

int settings::updateTime()
{
    return appsettings->option("basic.behaviour.updatetime")->value().toInt();
}