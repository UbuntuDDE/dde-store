#include "widgets/gallery.h"
#include <QHBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QTimer>

gallery::gallery(QList<QUrl> imageList)
{
    init();
    images = imageList;
    changeImage(0);
    pageIndicator->setPageCount(imageList.length());

    connect(backButton, &DIconButton::clicked, this, [ = ] {
        changeImage(currentImage - 1);
    });

    connect(forwardButton, &DIconButton::clicked, this, [ = ] {
        changeImage(currentImage + 1);
    });
}

gallery::gallery(QList<QPair<QString, App*>> imageList, MainWindow *parent)
{
    init();
    imageView->installEventFilter(this);
    banners = imageList;
    mainwindow = parent;
    changeLocalImage(0);
    pageIndicator->setPageCount(imageList.length());

    connect(backButton, &DIconButton::clicked, this, [ = ] {
        changeLocalImage(currentImage - 1);
    });

    connect(forwardButton, &DIconButton::clicked, this, [ = ] {
        changeLocalImage(currentImage + 1);
    });

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [ = ] {
        if (banners.length() - 1 == currentImage) {
            changeLocalImage(0);
        } else {
            changeLocalImage(currentImage + 1);
        }
    });
    timer->start(5000);
}

void gallery::init()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    QHBoxLayout *container = new QHBoxLayout;
    container->setAlignment(Qt::AlignTop);
    layout->addLayout(container);
    container->addStretch();

    backButton = new DIconButton(DStyle::SP_ArrowBack);
    backButton->setDisabled(true);
    container->addWidget(backButton, 1, Qt::AlignVCenter);
    
    imageView = new QLabel;
    container->addWidget(imageView);

    spinner = new DSpinner;
    spinner->setFixedSize(50, 50);
    spinner->hide();
    container->addWidget(spinner);

    forwardButton = new DIconButton(DStyle::SP_ArrowForward);
    forwardButton->setDisabled(true);
    container->addWidget(forwardButton, 1, Qt::AlignVCenter);

    container->addStretch();

    pageIndicator = new DPageIndicator;
    layout->addWidget(pageIndicator);
}

void gallery::changeLocalImage(int index)
{
    if (banners.length() != 1) {
        backButton->setDisabled(false);
        forwardButton->setDisabled(false);
    }
    
    if (index == 0) {
        backButton->setDisabled(true);
    } else if (index == banners.length() - 1) {
        forwardButton->setDisabled(true);
    }

    currentImage = index;
    pageIndicator->setCurrentPage(index);

    const int width = imageView->width() * devicePixelRatioF();
    QPixmap && pixmap = QPixmap::fromImage(QImage(banners[index].first)).scaled(width, width * 9/16, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap.setDevicePixelRatio(devicePixelRatioF());
    imageView->setPixmap(pixmap);
}

void gallery::changeImage(int index)
{
    if (images.length() != 1) {
        backButton->setDisabled(false);
        forwardButton->setDisabled(false);
    }
    
    if (index == 0) {
        backButton->setDisabled(true);
    } else if (index == images.length() - 1) {
        forwardButton->setDisabled(true);
    }

    currentImage = index;
    pageIndicator->setCurrentPage(index);

    QNetworkAccessManager *networkManager = new QNetworkAccessManager;
    QNetworkDiskCache *diskCache = new QNetworkDiskCache;
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    diskCache->setCacheDirectory(cacheDir);
    diskCache->setMaximumCacheSize(5242880);
    networkManager->setCache(diskCache);

    QNetworkRequest request;
    request.setUrl(images[index]);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    networkManager->get(request);

    imageView->hide();
    spinner->show();
    spinner->start();

    connect(networkManager, &QNetworkAccessManager::finished, this, [ = ] (QNetworkReply *reply) {
        QByteArray imageData = reply->readAll();
        QPixmap pixmap;
        pixmap.loadFromData(imageData);
        spinner->stop();
        spinner->hide();
        if (pixmap.isNull()) {
            if (images.length() == 1) {
                this->hide();
            } else {
                images.removeAt(index);
                pageIndicator->setPageCount(images.length());
                changeImage(0);
            }
        } else {
            imageView->setPixmap(pixmap.scaled(imageView->width(), imageView->width() * 9/16, Qt::AspectRatioMode::KeepAspectRatio, Qt::SmoothTransformation));
            imageView->show();
        }
    });
}

bool gallery::eventFilter(QObject *object, QEvent *event)
{
    if (object == imageView && !banners.isEmpty() && event->type() == QEvent::MouseButtonRelease) {
        mainwindow->openItem(banners[currentImage].second);
    }
    return QObject::eventFilter(object, event);
}
