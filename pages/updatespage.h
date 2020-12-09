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
    void loadData(QHash<QString, int> apps);
    void updatePercent(QString package, uint percent);
    void refresh();
private:
    List *list;
    DDialog *systemUpdatesPopup;
    QStringList appUpdates;
    QStringList systemUpdates;
    DSuggestButton *updateButton;
    MainWindow *mainwindow;
    QString systemUpdatesItem;
    DIconButton *refreshButton;
signals:
    void canRefresh();
    void cantRefresh();
};

#endif // UPDATEVIEW_H
