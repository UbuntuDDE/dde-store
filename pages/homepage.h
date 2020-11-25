#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include "mainwindow.h"

class HomePage : public QWidget
{
    Q_OBJECT

public:
    HomePage(MainWindow *parent);
private:
    void addCategory(QString name, QString category, MainWindow *parent);
    QVBoxLayout *layout;
    QStringList categories;
};

#endif // HOMEPAGE_H