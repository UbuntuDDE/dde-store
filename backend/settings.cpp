#include "settings.h"
#include <QStandardPaths>
#include <QApplication>
#include <qsettingbackend.h>
#include <DSettingsOption>

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

    appsettings->option("basic.view.nonapps")->setData("items", QStringList({tr("\"System Updates\""), tr("Individual Packages")}));
    appsettings->option("basic.behaviour.updatetime")->setData("items", QStringList({tr("Hourly"), tr("Never")}));
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

bool settings::popup()
{
    return appsettings->option("basic.behaviour.popup")->value().toBool();
}

int settings::updateTime()
{
    return appsettings->option("basic.behaviour.updatetime")->value().toInt();
}

void GenerateSettingTranslate()
{
    auto group_basicName = QObject::tr("Basic");
    auto group_basic_viewName = QObject::tr("View");
    auto group_basic_behaviourName = QObject::tr("Behaviour");
    auto group_basic_notificationsName = QObject::tr("Notifications");
    auto basic_view_nonapps = QObject::tr("Show non-app updates as");
    auto basic_view_maxitems = QObject::tr("Max apps per list");
    auto basic_behaviour_tray = QObject::tr("Minimize to tray on exit");
    auto basic_behaviour_popup = QObject::tr("Show the popup when the app still running in backgorund");
    auto basic_behaviour_updatetime = QObject::tr("Check for updates");
    auto basic_notifications_install = QObject::tr("App installed");
    auto basic_notifications_uninstall = QObject::tr("App uninstalled");
    auto basic_notifications_availableupdates = QObject::tr("Available updates");
    auto basic_notifications_finishedupdates = QObject::tr("Finished updates");
}