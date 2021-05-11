#ifndef LISTVIEW_H
#define LISTVIEW_H

#include "mainwindow.h"
#include "widgets/list.h"
#include <QWidget>

class CategoryPage : public QWidget
{
    Q_OBJECT
public:
    enum SortType {
        Alphabetical,
        Ratings
    };
    CategoryPage(MainWindow *parent, QString name, QString category);
    void insertItem(App *app);
    void load(SortType sort = Alphabetical);
private:
    void init(QString category, QString name);
    QList<App*> apps;
    List *list;
};

#endif // LISTVIEW_H
