#ifndef LISTVIEW_H
#define LISTVIEW_H

#include "mainwindow.h"
#include "widgets/list.h"
#include <QWidget>

class CategoryPage : public QWidget
{
    Q_OBJECT
public:
    CategoryPage(MainWindow *parent, QString name, QString category);
    void loadData(QStringList appList);
private:
    enum SortType {
        Alphabetical,
        Ratings
    };
    SortType sort = SortType::Alphabetical;
    QStringList apps;
    List *list;
};

#endif // LISTVIEW_H
