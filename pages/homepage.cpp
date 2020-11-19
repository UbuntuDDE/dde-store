#include "pages/homepage.h"
#include "backend/settings.h"
#include "backend/appstreamhelper.h"
#include "backend/ratingshelper.h"
#include "widgets/gallery.h"
#include "widgets/list.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMultiMap>
#include <DSuggestButton>
#include <DDialog>

DWIDGET_USE_NAMESPACE

HomePage::HomePage(MainWindow *parent)
{
    QScrollArea *scroll = new QScrollArea(this);
    QWidget *widget = new QWidget;
    scroll->setWidget(widget);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    widget->setLayout(layout);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    mainLayout->addWidget(scroll);

    QFile bannerFile("://resources/featuredbanners.json");
    bannerFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonObject bannerData = QJsonDocument::fromJson(bannerFile.readAll()).object();
    QList<QPixmap> bannerImages;
    for (const QString &key : bannerData.keys()) {
        bannerImages << QPixmap(bannerData.value(key).toString());
    }
    gallery *featuredGallery = new gallery(bannerImages, bannerData.keys(), parent);
    layout->addWidget(featuredGallery);

    addCategory("Messaging", "InstantMessaging", parent);
    addCategory("Internet", "Network", parent);
    addCategory("Games", "Game", parent);
    addCategory("Development", "Development", parent);
    addCategory("Office", "Office", parent);
    addCategory("Graphics", "Graphics", parent);
    addCategory("Video", "Video", parent);
    addCategory("Music", "Music", parent);
    addCategory("System", "System", parent);

}

void HomePage::addCategory(QString name, QString category, MainWindow *parent)
{
    categories << category;

    QStringList appList = AppStreamHelper::instance()->category(category);
    QMultiMap<double, QString> map;
    appList.sort();
    for (const QString app : appList) {
        AppStreamHelper::appData data = AppStreamHelper::instance()->getAppData(app);
        map.insert(RatingsHelper::instance()->totalRatings(data.id), app);
    }
    appList.clear();
    for (const QString app : map.values()) {
        appList.insert(0, app);
    }

    List *list = new List(name);
    list->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    list->listView->setFixedHeight(168);
    list->listView->verticalScrollBar()->setEnabled(false);
    list->listView->verticalScrollBar()->setVisible(false);
    list->listView->setOrientation(QListView::LeftToRight, true);
    
    DSuggestButton *moreBtn = new DSuggestButton("View More");
    connect(moreBtn, &DSuggestButton::clicked, this, [ = ] {
        parent->navView->setCurrentIndex(parent->navModel->index(categories.indexOf(category) + 1, 0));
    });
    list->addHeaderWidget(moreBtn);

    layout->addWidget(list);

    connect(list, &List::currentItemChanged, this, [ = ] (QString package) {
        parent->openItem(package);
    });

    for (QString app : appList) {
        list->addItem(app);
    }
    
    list->load();
}