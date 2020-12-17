#include "pages/categorypage.h"
#include "backend/appstreamhelper.h"
#include "backend/packagekithelper.h"
#include "backend/ratingshelper.h"
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
            sort = "alphabetical";
        } else if (text == sortRatings) {
            sort = "ratings";
        }
        loadData(apps);
    });
    list->addHeaderWidget(sortBox);

    connect(list, &List::currentItemChanged, this, [ = ] (QVariant data) {
        parent->openItem(data.toString());
    });

    if (category == "Installed") {
        PackageKitHelper::instance()->getInstalled(this);
    } else if (name.startsWith("\"")) {
        loadData(AppStreamHelper::instance()->search(category));
        list->setEmptyText(tr("No results for %1").arg(name));
    } else {
        loadData(AppStreamHelper::instance()->category(category));
    }

    layout->addWidget(list);
}

void CategoryPage::loadData(QStringList appList)
{
    list->clear();   
    if (sort == "alphabetical") {
        appList.sort();
    } else if (sort == "ratings") {
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
    }
    apps = appList;
    for (const QString &app : appList) {
        AppStreamHelper::appData data = AppStreamHelper::instance()->getAppData(app);
        list->addItem(data.name, data.icon, app);
    }
    list->load();
}
