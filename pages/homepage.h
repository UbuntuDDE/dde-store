#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include "mainwindow.h"

class HomePage : public QWidget
{
    Q_OBJECT

public:
    HomePage(MainWindow *parent);
};

#endif // HOMEPAGE_H