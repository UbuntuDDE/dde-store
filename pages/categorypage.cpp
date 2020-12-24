#include "pages/categorypage.h"
#include "backend/appstreamhelper.h"
#include "backend/packagekithelper.h"
#include "backend/ratingshelper.h"
#include <QComboBox>
#include <QVBoxLayout>
#ifdef SNAP
#include "backend/snaphelper.h"
#endif

CategoryPage::CategoryPage(MainWindow *parent, QString name, QString category)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);
    list = new List(name);
    DLabel *sortLabel = new DLabel(tr("Sort by"));
    list->addHeaderWidget(sortLabel);
    QComboBox *sortBox = new QComboBox;
    QString sortAlphabetical = tr("Alphabetical");
    QString sortRatings = tr("Ratings");
    sortBox->addItems(QStringList() << sortAlphabetical << sortRatings);
    connect(sortBox, &QComboBox::currentTextChanged, this, [ = ] (const QString &text) {
        if (text == sortAlphabetical) {
            load(Alphabetical);
        } else if (text == sortRatings) {
            load(Ratings);
        }
    });
    list->addHeaderWidget(sortBox);

    connect(list, &List::currentItemChanged, this, [ = ] (QVariant data) {
        bool snap = false;
        for (App app : apps) {
            if (app.id == data.toString()) {
                if (app.source == Snap) {
                    snap = true;
                }
            }
        }
        parent->openItem(data.toString(), snap); 
    });

    if (category == "Installed") {
        PackageKitHelper::instance()->getInstalled(this);
    } else if (name.startsWith("\"")) {
        list->setEmptyText(tr("No results for %1").arg(name));
        auto list = AppStreamHelper::instance()->search(category);
        for (QString entry : list) {
            auto app = AppStreamHelper::instance()->getAppData(entry);
            App item;
            item.name = app.name;
            item.icon = app.icon;
            item.id = entry;
            item.ratings = RatingsHelper::instance()->totalRatings(item.id);
            item.source = PackageKit;
            insertItem(item);
        }
#ifdef SNAP
        SnapHelper::instance()->search(this, category);
#else
        load();
#endif
    } else {
        auto list = AppStreamHelper::instance()->category(category);
        for (QString entry : list) {
            auto app = AppStreamHelper::instance()->getAppData(entry);
            App item;
            item.name = app.name;
            item.icon = app.icon;
            item.id = entry;
            item.ratings = RatingsHelper::instance()->totalRatings(item.id);
            item.source = PackageKit;
            insertItem(item);
        }
        load();
    }

    layout->addWidget(list);
}

void CategoryPage::insertItem(App item)
{
    list->addItem(item.name, item.icon, item.id);
    apps << item;
}

void CategoryPage::load(SortType sort)
{
    list->clear();
    if (sort == Alphabetical) {
        QMap<QString, App> map;
        for (App item : apps) {
            map.insert(item.name, item);
        }
        apps = map.values();
    } else {
        QMap<int, App> map;
        for (App item : apps) {
            map.insert(item.ratings, item);
        }
        QList<App> list;
        for (App item : map.values()) {
            list << item;
        }
        apps = list;
    }
    for (App item : apps) {
        insertItem(item);
    }
    list->load();
}