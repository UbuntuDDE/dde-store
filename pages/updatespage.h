#ifndef UPDATEVIEW_H
#define UPDATEVIEW_H

#include "mainwindow.h"
#include "widgets/list.h"
#include <QWidget>
#include <DDialog>
#include <DSuggestButton>
#include <DIconButton>

DWIDGET_USE_NAMESPACE

class UpdatesPage : public QWidget
{
    Q_OBJECT
public:
    UpdatesPage(MainWindow *parent);
    void insertSystemUpdate(QString id, int size);
    void insertItem(QString name, QIcon icon, QString id, int size);
    void load();
    void updatePercent(QString package, uint percent);
    void refresh(bool refreshCache = false);
private:
    List *list;
    DDialog *systemUpdatesPopup;
    QStringList appUpdates;
    QStringList updates;
    DSuggestButton *updateButton;
    MainWindow *mainwindow;
    DIconButton *refreshButton;
    int totalSize = 0;
signals:
    void canRefresh();
    void cantRefresh();
};

#endif // UPDATEVIEW_H
