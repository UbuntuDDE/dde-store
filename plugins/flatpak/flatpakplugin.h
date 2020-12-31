#ifndef FLATPAKPLUGIN_H
#define FLATPAKPLUGIN_H

#include <QObject>
#include <plugins/flatpak/flatpakinterface.h>
extern "C" {
#include <flatpak.h>
}

class FlatpakPlugin : public FlatpakInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.dekzi.dde-store.FlatpakPlugin")
    Q_INTERFACES(FlatpakInterface)

public:
    FlatpakPlugin();

private:


};

#endif // FLATPAKPLUGIN_H