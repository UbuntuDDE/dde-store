#include "widgets/list.h"
#include "backend/appstreamhelper.h"
#include <DSpinner>

List::List(QString title)
{
    layout = new QVBoxLayout;
    this->setLayout(layout);

    QWidget *header = new QWidget;
    headerLayout = new QHBoxLayout;
    header->setLayout(headerLayout);
    DLabel *titleLabel = new DLabel(title);
    QFont titleFont;
    titleFont.setPixelSize(28);
    titleLabel->setFont(titleFont);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->setSpacing(10);

    listView = new DListView;
    listView->setViewMode(QListView::IconMode);
    listView->setItemSize(QSize(128,128));
    listView->setIconSize(QSize(64, 64));
    listView->setSpacing(20);
    listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    listView->setSelectionMode(QAbstractItemView::NoSelection);
    listView->setMaximumWidth(QWIDGETSIZE_MAX);
    listView->setMinimumWidth(188);
    listView->setEditTriggers(QListView::NoEditTriggers);
    listView->setResizeMode(QListView::Adjust);
    listView->setOrientation(QListView::LeftToRight, true);
    DStyle::setFrameRadius(listView, 18);
    model = new QStandardItemModel;
    listView->setModel(model);
    layout->addWidget(header, 0, Qt::AlignTop);

    connect(listView, qOverload<const QModelIndex &>(&DListView::currentChanged), this, [ = ] {
        if (listView->currentIndex().row() != -1) {
            emit currentItemChanged(model->item(listView->currentIndex().row())->data().toString());
            listView->setCurrentIndex(QModelIndex());
        }
    });

    loadingBox = new QWidget;
    QHBoxLayout *loadingBoxLayout = new QHBoxLayout;
    loadingBox->setLayout(loadingBoxLayout);
    DSpinner *loading = new DSpinner;
    loading->setFixedSize(50, 50);
    loading->start();
    loadingBoxLayout->addWidget(loading);
    layout->addWidget(loadingBox, 0, Qt::AlignTop);

    emptyLabel = new DLabel;
    QFont emptyFont;
    emptyFont.setPixelSize(28);
    emptyLabel->setFont(emptyFont);
    emptyLabel->setAlignment(Qt::AlignHCenter);
    emptyLabel->hide();
    layout->addWidget(emptyLabel, 0, Qt::AlignTop);
}

void List::addItem(QString package)
{
    AppStreamHelper::appData data = AppStreamHelper::instance()->getAppData(package);
    DStandardItem *item = new DStandardItem(data.icon, data.name);
    item->setData(package);
    model->appendRow(item);
}

void List::addItem(QString name, QIcon icon)
{
    DStandardItem *item = new DStandardItem(icon, name);
    item->setData(name);
    model->appendRow(item);
}

void List::addHeaderWidget(QWidget *widget)
{
    headerLayout->addWidget(widget);
}

void List::setEmptyText(QString text)
{
    emptyLabel->setText(text);
}

void List::editItemText(QString package, QString text)
{
    for (int i = 0; i < model->rowCount(); i++) {
        if (model->item(i)->data() == package) {
            model->item(i)->setText(text);
        }
    }
}

void List::clear()
{
    model->clear();
    listView->removeItems(0, listView->count());
}

void List::load()
{
    loadingBox->hide();
    if (model->rowCount() == 0) {
        layout->removeWidget(listView);
        emptyLabel->show();
    } else {
        layout->addWidget(listView);
    }
}