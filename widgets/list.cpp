#include "widgets/list.h"

List::List(QString title)
{
    layout = new QVBoxLayout;
    this->setLayout(layout);

    headerLayout = new QHBoxLayout;
    headerLayout->setAlignment(Qt::AlignTop);
    DLabel *titleLabel = new DLabel(title);
    QFont titleFont;
    titleFont.setPixelSize(28);
    titleLabel->setFont(titleFont);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->setSpacing(10);
    headerLayout->setMargin(10);
    layout->addLayout(headerLayout);

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

    connect(listView, qOverload<const QModelIndex &>(&DListView::currentChanged), this, [ = ] {
        if (listView->currentIndex().row() != -1) {
            emit currentItemChanged(model->item(listView->currentIndex().row())->data());
            listView->setCurrentIndex(QModelIndex());
        }
    });

    loading = new DSpinner;
    loading->setFixedSize(50, 50);
    loading->start();
    layout->addWidget(loading, 0, Qt::AlignHCenter | Qt::AlignTop);

    emptyLabel = new DLabel;
    QFont emptyFont;
    emptyFont.setPixelSize(28);
    emptyLabel->setFont(emptyFont);
    emptyLabel->setAlignment(Qt::AlignHCenter);
    emptyLabel->hide();
    layout->addWidget(emptyLabel, 0, Qt::AlignTop);
}

void List::addItem(QString name, QIcon icon, QVariant data)
{
    DStandardItem *item = new DStandardItem(icon, name);
    item->setData(data.isNull() ? name : data);
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
    loading->hide();
    if (model->rowCount() == 0) {
        layout->removeWidget(listView);
        emptyLabel->show();
    } else {
        layout->addWidget(listView);
    }
}

void List::unload()
{
    loading->show();
    emptyLabel->hide();
    layout->removeWidget(listView);
}