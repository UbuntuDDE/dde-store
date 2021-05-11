#include "pages/categorypage.h"
#include "backend/sources/packagekit/packagekitsource.h"
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
        for (App *app : apps) {
            if (app->id == data.toString()) {
                parent->openItem(app);
                break;
            }
        }
    });

    layout->addWidget(list);

    init(category, name);
}

void CategoryPage::init(QString category, QString name)
{
    if (category == "Installed") {
        for (Source *source : SourceManager::instance()->sources()) {
            connect(source, &Source::gotInstalled, this, [ = ] (QList<App*> installed) {
                for (App *app : installed) {
                    insertItem(app);
                }
                if (SourceManager::instance()->sources().endsWith(source))
                    load();
                else
                    SourceManager::instance()->sources()[SourceManager::instance()->sources().indexOf(source) + 1]->getInstalled();
            });
            source->getInstalled();
        }
        SourceManager::instance()->sources().first()->getInstalled();
    } else if (name.startsWith("\"")) {
        list->setEmptyText(tr("No results for %1").arg(name));
        for (Source *source : SourceManager::instance()->sources()) {
            connect(source, &Source::searchFinished, this, [ = ] (QList<App*> results) {
                if (SourceManager::instance()->sources().startsWith(source)) {
                    list->clear();
                    apps.clear();
                }
                for (App *app : results) {
                    insertItem(app);
                }
                if (SourceManager::instance()->sources().endsWith(source))
                    load();
                else
                    SourceManager::instance()->sources()[SourceManager::instance()->sources().indexOf(source) + 1]->search(category);
            });
        }
        SourceManager::instance()->sources().first()->search(category);
    } else {
        PackageKitSource *pk = static_cast<PackageKitSource*>(SourceManager::instance()->getSource("PackageKit"));
        connect(pk, &PackageKitSource::gotCategory, this, [ = ] (QString cat, QList<App*> items) {
            if (cat == category) {
                for (App *app : items) {
                    insertItem(app);
                }
                load();
            }
        });
        pk->getCategory(category);
    }
}

void CategoryPage::insertItem(App *app)
{
    list->addItem(app->name, app->icon, app->id);
    apps << app;
}

void CategoryPage::load(SortType sort)
{
    list->clear();
    if (sort == Alphabetical) {
        QMap<QString, App*> map;
        for (App *app : apps) {
            map.insert(app->name.toLower(), app);
        }
        apps.clear();
        for (App *app : map.values()) {
            insertItem(app);
        }
    } else {
        QMultiMap<int, App*> map;
        for (App *app : apps) {
            map.insert(app->ratings, app);
        }
        QList<App*> appList;
        for (App *app : map.values()) {
            appList.insert(0, app);
        }
        apps.clear();
        for (App *app : appList) {
            insertItem(app);
        }
    }
    list->load();
}