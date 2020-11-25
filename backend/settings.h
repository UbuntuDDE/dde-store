#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <DSettings>

DCORE_USE_NAMESPACE

class settings : public QObject
{
    Q_OBJECT

public:
    static settings *instance();
    DSettings *appsettings;
    int maxItems();
    int nonApps();
    bool notifyInstall();
    bool notifyUninstall();
    bool notifyAvailableUpdates();
    bool notifyFinishedUpdates();

private:
    static settings *currentInstance;
    settings();
};

#endif // SETTINGS_H