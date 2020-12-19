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
#include <QMultiMap>
#include <DSuggestButton>

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
    QList<QPair<QString, QString>> banners;
    for (const QString &key : bannerData.keys()) {
        if (!AppStreamHelper::instance()->packageFromID(key).isEmpty()) {
            QPair<QString, QString> banner;
            banner.first = bannerData.value(key).toString();
            banner.second = AppStreamHelper::instance()->packageFromID(key);
            banners << banner;
        }
    }
    gallery *featuredGallery = new gallery(banners, parent);
    layout->addWidget(featuredGallery);

    addCategory(tr("Messaging"), "InstantMessaging", parent);
    addCategory(tr("Internet"), "Network", parent);
    addCategory(tr("Games"), "Game", parent);
    addCategory(tr("Development"), "Development", parent);
    addCategory(tr("Office"), "Office", parent);
    addCategory(tr("Graphics"), "Graphics", parent);
    addCategory(tr("Video"), "Video", parent);
    addCategory(tr("Music"), "Music", parent);
    addCategory(tr("System"), "System", parent);
}

void HomePage::addCategory(QString name, QString category, MainWindow *parent)
{
    categories << category;

    List *list = new List(name);
    list->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    list->listView->setFixedHeight(168);
    list->listView->verticalScrollBar()->setEnabled(false);
    list->listView->verticalScrollBar()->setVisible(false);
    list->listView->setOrientation(QListView::LeftToRight, true);
    
    connect(RatingsHelper::instance(), &RatingsHelper::fetched, this, [ = ] {
        QStringList appList = AppStreamHelper::instance()->category(category);
        QMultiMap<double, QString> map;
        appList.sort();
        for (const QString &app : appList) {
            AppStreamHelper::appData data = AppStreamHelper::instance()->getAppData(app);
            map.insert(RatingsHelper::instance()->totalRatings(data.id), app);
        }
        appList.clear();
        for (const QString &app : map.values()) {
            appList.insert(0, app);
        }
        for (QString app : appList) {
            AppStreamHelper::appData data = AppStreamHelper::instance()->getAppData(app);
            list->addItem(data.name, data.icon, app);
        }
    
        list->load();
    });
    
    DSuggestButton *moreBtn = new DSuggestButton(tr("View More"));
    connect(moreBtn, &DSuggestButton::clicked, this, [ = ] {
        parent->navView->setCurrentIndex(parent->navModel->index(categories.indexOf(category) + 1, 0));
    });
    list->addHeaderWidget(moreBtn);

    layout->addWidget(list);

    connect(list, &List::currentItemChanged, this, [ = ] (QVariant data) {
        parent->openItem(data.toString());
    });
}
