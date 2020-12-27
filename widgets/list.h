#ifndef LIST_H
#define LIST_H

#include <QWidget>
#include <QVBoxLayout>
#include <DListView>
#include <DStandardItem>
#include <DLabel>
#include <DSpinner>

DWIDGET_USE_NAMESPACE

class List : public QWidget
{
    Q_OBJECT
public:
    List(QString title);
    void addItem(QString name, QIcon icon, QVariant data = QVariant());
    void addHeaderWidget(QWidget *widget);
    void setEmptyText(QString text);
    void editItemText(QString package, QString text);
    void clear();
    void load();
    void unload();
    DListView *listView;
private:
    QHBoxLayout *headerLayout;
    QStandardItemModel *model;
    DSpinner *loading;
    DLabel *emptyLabel;
    QVBoxLayout *layout;
signals:
    void currentItemChanged(QVariant data);
};

#endif // LIST_H
