#include "pages/homepage.h"
#include "backend/settings.h"
#include "backend/sourcemanager.h"
#include "backend/sources/packagekit/packagekitsource.h"
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

    QFile bannerFile("://featuredbanners.json");
    bannerFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonObject bannerData = QJsonDocument::fromJson(bannerFile.readAll()).object();
    QList<QPair<QString, App*>> banners;
    for (const QString &key : bannerData.keys()) {
        App *app = static_cast<PackageKitSource*>(SourceManager::instance()->getSource("PackageKit"))->getDataFromID(key);
        if (app->hasMetadata)
            banners << qMakePair(bannerData.value(key).toString(), app);
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

    PackageKitSource *pk = static_cast<PackageKitSource*>(SourceManager::instance()->getSource("PackageKit"));
    QList<App*> *apps = new QList<App*>();
    connect(pk, &PackageKitSource::gotCategory, this, [ = ] (QString cat, QList<App*> items) {
        if (cat == category) {
            QMultiMap<int, App*> map;
            for (App *app : items) {
                map.insert(app->ratings, app);
            }
            items.clear();
            for (App *app : map.values()) {
                items.insert(0, app);
            }
            for (int i = 0; i < 10; i++) {
                list->addItem(items[i]->name, items[i]->icon, i);
                apps->append(items[i]);
            }
            list->load();
            disconnect(pk, &PackageKitSource::gotCategory, this, nullptr);
        }
    });
    pk->getCategory(category);
    
    DSuggestButton *moreBtn = new DSuggestButton(tr("View More"));
    connect(moreBtn, &DSuggestButton::clicked, this, [ = ] {
        parent->navView->setCurrentIndex(parent->navModel->index(categories.indexOf(category) + 1, 0));
    });
    list->addHeaderWidget(moreBtn);

    layout->addWidget(list);

    connect(list, &List::currentItemChanged, this, [ = ] (QVariant data) {
        parent->openItem(apps->value(data.toInt()));
    });
}
