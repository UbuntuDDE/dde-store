#include "pages/itempage.h"
#include "backend/appstreamhelper.h"
#include "backend/packagekithelper.h"
#include "backend/ratingshelper.h"
#include "widgets/gallery.h"
#include "widgets/stars.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <DLabel>

ItemPage::ItemPage(QString app)
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

    PackageKitHelper::instance()->itemPageData(this, app);
    AppStreamHelper::appData data = AppStreamHelper::instance()->getAppData(app);

    QWidget *headerSection = new QWidget;
    QHBoxLayout *headerSectionLayout = new QHBoxLayout;
    headerSection->setLayout(headerSectionLayout);

    QLabel *icon = new QLabel;
    icon->setPixmap(data.icon.pixmap(data.icon.actualSize(QSize(64, 64))));
    headerSectionLayout->addWidget(icon);

    QWidget *nameSection = new QWidget;
    QVBoxLayout *nameSectionLayout = new QVBoxLayout;
    nameSection->setLayout(nameSectionLayout);
    nameSectionLayout->addStretch();
    DLabel *name = new DLabel(data.name);
    QFont nameFont;
    nameFont.setPixelSize(22);
    name->setFont(nameFont);
    nameSectionLayout->addWidget(name);
    if (!data.developer.isNull()) {
        DLabel *developer = new DLabel(data.developer);
        nameSectionLayout->addWidget(developer);
    }
    if (RatingsHelper::instance()->averageRating(data.id) != 0) {
        stars *starsView = new stars(data.id);
        nameSectionLayout->addWidget(starsView);
    }
    nameSectionLayout->addStretch();
    headerSectionLayout->addWidget(nameSection);

    headerSectionLayout->addStretch();

    removeBtn = new DWarningButton;
    removeBtn->setText("Uninstall");
    removeBtn->hide();
    headerSectionLayout->addWidget(removeBtn, 0, Qt::AlignVCenter);
    headerSectionLayout->addSpacing(10);
    installBtn = new DSuggestButton;
    installBtn->hide();
    headerSectionLayout->addWidget(installBtn, 0, Qt::AlignVCenter);

    layout->addWidget(headerSection, 0, Qt::AlignTop);

    progressBar = new DProgressBar;
    progressBar->hide();
    layout->addWidget(progressBar, 0, Qt::AlignTop);
    
    if (!data.screenshots.isEmpty()) {
        gallery *screenshotView = new gallery(data.screenshots);
        layout->addWidget(screenshotView);
    }

    DLabel *description = new DLabel;
    description->setWordWrap(true);
    description->setAlignment(Qt::AlignHCenter);
    description->setMargin(15);
    if (data.description.isNull()) {
        description->setText("<i>No description provided.</i>");
    } else {
        description->setText(data.description);
    }

    layout->addWidget(description);
}

void ItemPage::setInstallButton(QString packageId, QString type, QString param)
{
    if (type == "notinstalled") {
        removeBtn->hide();
        progressBar->hide();
        installBtn->show();
        installBtn->setDisabled(false);
        installBtn->setText(QString("Install (%1)").arg(param));
        installBtn->disconnect(this);
        connect(installBtn, &DSuggestButton::clicked, this, [ = ] {
            PackageKitHelper::instance()->install(this, packageId);
        });
    } else if (type == "launchable") {
        progressBar->hide();
        installBtn->show();
        installBtn->setDisabled(false);
        installBtn->setText("Open");
        installBtn->disconnect(this);
        connect(installBtn, &DSuggestButton::clicked, this, [ = ] {
            PackageKitHelper::instance()->launch(packageId);
        });
        removeBtn->show();
        connect(removeBtn, &DWarningButton::clicked, this, [ = ] {
            PackageKitHelper::instance()->uninstall(this, packageId);
        });
    } else if (type == "installed") {
        progressBar->hide();
        installBtn->hide();
        removeBtn->show();
        connect(removeBtn, &DWarningButton::clicked, this, [ = ] {
            PackageKitHelper::instance()->uninstall(this, packageId);
        });
    } else if (type == "installing") {
        progressBar->show();
        progressBar->setValue(param.toInt());
        removeBtn->hide();
        installBtn->setDisabled(true);
    }
}
