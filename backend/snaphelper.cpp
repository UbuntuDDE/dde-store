#include "snaphelper.h"
#include <QLocale>
#include <QBuffer>
#include <QImageReader>

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
    auto request = client->find(QSnapdClient::MatchName, app);
    request->runAsync();
    connect(request, &QSnapdRequest::complete, this, [ = ] {
        AppStreamHelper::appData data;
        data.name = request->snap(0)->title();
        data.description = request->snap(0)->description();
        data.summary = request->snap(0)->summary();
        data.developer = request->snap(0)->publisherDisplayName();
        QList<QUrl> screenshots;
        for (int i = 0; i < request->snap(0)->mediaCount(); i++) {
            if (request->snap(0)->media(i)->type() == "screenshot") {
                screenshots << QUrl(request->snap(0)->media(i)->url());
            }
        }
        data.screenshots = screenshots;
        data.id = request->snap(0)->name();

        if (request->snap(0)->icon().startsWith("/")) {
            data.icon = QIcon(request->snap(0)->icon());
        } else if (request->snap(0)->icon().isEmpty()) {
            data.icon = QIcon::fromTheme("application-x-executable");
        } else {
            auto iconrequest = client->getIcon(app);
            iconrequest->runSync();
            if (!iconrequest->error()) {
                QBuffer *buffer;
                buffer->setData(iconrequest->icon()->data());
                QImageReader reader(buffer);
                reader.read();
                data.icon = QIcon(QPixmap::fromImage(reader.read()));
            }
        }

        if (data.icon.isNull()) {
            data.icon = QIcon::fromTheme("application-x-executable");
        }

        page->setData(data);

        switch (request->snap(0)->status()) {
            case QSnapdEnums::SnapStatusAvailable:
                page->setInstallButton(data.id, ItemPage::NotInstalled, QLocale().formattedDataSize(request->snap(0)->downloadSize()));
                break;
            case QSnapdEnums::SnapStatusInstalled:
                page->setInstallButton(data.id, ItemPage::Installed);
                break;
        };
    });
}