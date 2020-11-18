#ifndef LISTVIEW_H
#define LISTVIEW_H

#include "mainwindow.h"
#include "widgets/list.h"
#include <QWidget>
#include <DStandardItem>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class CategoryPage : public QWidget
{
    Q_OBJECT
public:
    CategoryPage(MainWindow *parent, QString name, QString category = nullptr);
    void loadData(QStringList appList);
private:
    QString sort = "alphabetical";
    QStringList apps;
    List *list;
};

#endif // LISTVIEW_H
