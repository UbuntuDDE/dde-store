#include "snaphelper.h"

SnapHelper *SnapHelper::currentInstance = nullptr;

SnapHelper *SnapHelper::instance()
{
    if (!currentInstance) {
        currentInstance = new SnapHelper;
    }
    return currentInstance;
}

SnapHelper::SnapHelper()
{
    client = new QSnapdClient;
}

void SnapHelper::itemPageData(ItemPage *page, QString app)
{
    auto request = client->getSnap(app);
    request->runAsync();
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        AppStreamHelper::appData data;
        data.name = request->snap()->title();
        data.icon = QIcon(request->snap()->icon());
        data.description = request->snap()->description();
        data.summary = request->snap()->summary();
        data.developer = request->snap()->publisherDisplayName();
        QList<QUrl> screenshots;
        for (int i = 0; i < request->snap()->screenshotCount(); i++) {
            screenshots << QUrl(request->snap()->screenshot(i)->url());
        }
        data.screenshots = screenshots;
        data.id = request->snap()->name();
        page->setData(data);

        switch (request->snap()->status()){
            case QSnapdEnums::SnapStatusAvailable:
                page->setInstallButton(data.id, ItemPage::NotInstalled);
                break;
            case QSnapdEnums::SnapStatusInstalled:
                page->setInstallButton(data.id, ItemPage::Installed);
                break;
        }
    });
}