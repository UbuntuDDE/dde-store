#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DListView>
#include <QStackedWidget>
#include <QKeyEvent>
#include "pages/itempage.h"

DWIDGET_USE_NAMESPACE

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void openItem(QString app);
    void setUpdateIndicatorVisible(bool value);

private:
    void initNav();
    void initTitlebar();
    void addPage(QString name, QString icon, QWidget *widget);
    DListView *navView = new DListView;
    QStandardItemModel *navModel = new QStandardItemModel;
    QStackedWidget *stackedWidget = new QStackedWidget;
    QHash<QString, QString> pageIcons;
    QHash<QString, ItemPage*> itemPageList;
    DViewItemAction *updateIndicator;
    
protected Q_SLOTS:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
