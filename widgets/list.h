#ifndef LIST_H
#define LIST_H

#include <QWidget>
#include <QVBoxLayout>
#include <DListView>
#include <DStandardItem>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class List : public QWidget
{
    Q_OBJECT
public:
    List(QString title);
    void addItem(QString package);
    void addItem(QString name, QIcon icon);
    void addHeaderWidget(QWidget *widget);
    void setEmptyText(QString text);
    void editItemText(QString package, QString text);
    void clear();
    void load();
    DListView *listView;
private:
    QHBoxLayout *headerLayout;
    QStandardItemModel *model;
    QWidget *loadingBox;
    DLabel *emptyLabel;
    QVBoxLayout *layout;
signals:
    void currentItemChanged(QString package);
};

#endif // LIST_H
