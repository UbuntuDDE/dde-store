#ifndef FLATPAKINTERFACE_H
#define FLATPAKINTERFACE_H

#include <QObject>

class FlatpakInterface : public QObject
{
    Q_OBJECT

public:
    
};

Q_DECLARE_INTERFACE(FlatpakInterface, "org.dekzi.dde-store.FlatpakInterface")

#endif // FLATPAKINTERFACE_H