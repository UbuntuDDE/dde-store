#include "pages/homepage.h"
#include "backend/settings.h"
#include "backend/appstreamhelper.h"
#include "widgets/gallery.h"
#include "widgets/list.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
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
    QVBoxLayout *layout = new QVBoxLayout;
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

    QFile appsFile("://resources/featuredapps.json");
    appsFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonObject featuredData = QJsonDocument::fromJson(appsFile.readAll()).object();
    appsFile.close();

    for (const QString &key : featuredData.keys()) {
        List *list = new List(key);
        list->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        list->listView->setFixedHeight(168);
        list->listView->verticalScrollBar()->setEnabled(false); // ?????
        list->listView->setOrientation(QListView::LeftToRight, true);
        
        DSuggestButton *moreBtn = new DSuggestButton("View More");
        connect(moreBtn, &DSuggestButton::clicked, this, [ = ] {
            DDialog dialog(key, "adslkjdf");
            dialog.setIcon(DStyle().standardIcon(QStyle::SP_MessageBoxInformation));
            dialog.exec();
        });
        list->addHeaderWidget(moreBtn);

        layout->addWidget(list);

        connect(list, &List::currentItemChanged, this, [ = ] (QString package) {
            parent->openItem(package);
        });

        for (const QJsonValue &value : featuredData.value(key).toArray()) {
            if (AppStreamHelper::instance()->hasAppData(value.toString())) {
                list->addItem(value.toString());
            }
        }

        list->load();
    }
}
