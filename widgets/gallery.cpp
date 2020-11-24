#include "widgets/gallery.h"
#include <QHBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QTimer>

gallery::gallery(QList<QUrl> imageList)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    this->setLayout(layout);
    QHBoxLayout *widgetLayout = new QHBoxLayout;
    QWidget *widget = new QWidget;
    layout->addWidget(widget);
    widget->setLayout(widgetLayout);
    images = imageList;
    layout->setContentsMargins(0, 0, 0, 0);

    widgetLayout->addStretch();

    backButton = new DIconButton(DStyle::SP_ArrowBack);
    connect(backButton, &DIconButton::clicked, this, [ = ] {
        changeImage(currentImage - 1);
    });
    backButton->setDisabled(true);
    widgetLayout->addWidget(backButton, 0, Qt::AlignVCenter);
    
    imageView = new QLabel;
    widgetLayout->addWidget(imageView);

    spinner = new DSpinner;
    widgetLayout->addWidget(spinner);
    spinner->setFixedSize(50, 50);
    spinner->hide();

    forwardButton = new DIconButton(DStyle::SP_ArrowForward);
    connect(forwardButton, &DIconButton::clicked, this, [ = ] {
        changeImage(currentImage + 1);
    });
    forwardButton->setDisabled(true);
    widgetLayout->addWidget(forwardButton, 0, Qt::AlignVCenter);

    widgetLayout->addStretch();

    pageIndicator = new DPageIndicator;
    pageIndicator->setPageCount(images.length());
    layout->addWidget(pageIndicator);

    changeImage(0);
}

gallery::gallery(QList<QPair<QPixmap, QString>> imageList, MainWindow *parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    this->setLayout(layout);
    QHBoxLayout *widgetLayout = new QHBoxLayout;
    QWidget *widget = new QWidget;
    layout->addWidget(widget);
    widget->setLayout(widgetLayout);
    banners = imageList;
    mainwindow = parent;
    layout->setContentsMargins(0, 0, 0, 0);

    widgetLayout->addStretch();

    backButton = new DIconButton(DStyle::SP_ArrowBack);
    connect(backButton, &DIconButton::clicked, this, [ = ] {
        changeLocalImage(currentImage - 1);
    });
    backButton->setDisabled(true);
    widgetLayout->addWidget(backButton, 0, Qt::AlignVCenter);
    
    imageView = new QLabel;
    widgetLayout->addWidget(imageView);
    imageView->installEventFilter(this);

    spinner = new DSpinner;
    widgetLayout->addWidget(spinner);
    spinner->setFixedSize(50, 50);
    spinner->hide();

    forwardButton = new DIconButton(DStyle::SP_ArrowForward);
    connect(forwardButton, &DIconButton::clicked, this, [ = ] {
        changeLocalImage(currentImage + 1);
    });
    forwardButton->setDisabled(true);
    widgetLayout->addWidget(forwardButton, 0, Qt::AlignVCenter);

    widgetLayout->addStretch();

    pageIndicator = new DPageIndicator;
    pageIndicator->setPageCount(banners.length());
    layout->addWidget(pageIndicator);

    changeLocalImage(0);

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

    imageView->setPixmap(banners[index].first.scaled(imageView->width(), imageView->width() * 9/16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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