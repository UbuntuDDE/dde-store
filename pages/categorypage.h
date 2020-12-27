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
    enum Backend {
        PackageKit,
        Snap
    };
    struct App
    {
        QString name;
        QIcon icon;
        QString id;
        QString package;
        int ratings;
        Backend source;
    };
    CategoryPage(MainWindow *parent, QString name, QString category);
    void insertItem(App item);
    void load(SortType sort = Alphabetical);
private:
    void init(QString category, QString name);
    QList<CategoryPage::App> apps;
    List *list;
};

#endif // LISTVIEW_H
