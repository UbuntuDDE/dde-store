#include "pages/categorypage.h"
#include "backend/appstreamhelper.h"
#include "backend/packagekithelper.h"
#include "backend/ratingshelper.h"
#include "plugins/pluginloader.h"
#include <QComboBox>
#include <QVBoxLayout>

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
        for (App app : apps) {
            if (app.id == data.toString()) {
                parent->openItem(app.package, app.id, (app.source == Snap));
            }
        }
    });

    if (RatingsHelper::instance()->available) {
        init(category, name);
    } else {
        connect(RatingsHelper::instance(), &RatingsHelper::fetched, this, [ = ] {
            init(category, name);
        });
    }
    layout->addWidget(list);
}

void CategoryPage::init(QString category, QString name)
{
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
            item.id = app.id;
            item.package = entry;
            item.ratings = RatingsHelper::instance()->totalRatings(app.id);
            item.source = PackageKit;
            insertItem(item);
        }
        if (PluginLoader::instance()->snapPlugin) {
            PluginLoader::instance()->snapPlugin->search(this, category);
        } else {
            load();
        }
    } else {
        auto list = AppStreamHelper::instance()->category(category);
        for (QString entry : list) {
            auto app = AppStreamHelper::instance()->getAppData(entry);
            App item;
            item.name = app.name;
            item.icon = app.icon;
            item.id = app.id;
            item.package = entry;
            item.ratings = RatingsHelper::instance()->totalRatings(app.id);
            item.source = PackageKit;
            insertItem(item);
        }
        load();
    }
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
            map.insert(item.name.toLower(), item);
        }
        apps.clear();
        for (App item : map.values()) {
            insertItem(item);
        }
    } else {
        QMultiMap<int, App> map;
        for (App item : apps) {
            map.insert(item.ratings, item);
        }
        QList<App> list;
        for (App item : map.values()) {
            list.insert(0, item);
        }
        apps.clear();
        for (App item : list) {
            insertItem(item);
        }
    }
    list->load();
}