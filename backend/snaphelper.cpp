#include "snaphelper.h"

SnapHelper *SnapHelper::currentInstance = nullptr;

SnapHelper *SnapHelper::instance()
{
    if (!currentInstance) {
        currentInstance = new SnapHelper;
    }
    return currentInstance;
}

SnapHelper::SnapHelper()
{
    client = new QSnapdClient;
}