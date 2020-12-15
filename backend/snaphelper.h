#ifndef SNAPHELPER_H
#define SNAPHELPER_H

#include <QObject>
#include <Snapd/Client>

class SnapHelper : public QObject
{
    Q_OBJECT

public:
    static SnapHelper *instance();

private:
    static SnapHelper *currentInstance;
    SnapHelper();
    QSnapdClient *client;
};

#endif // SNAPHELPER_H