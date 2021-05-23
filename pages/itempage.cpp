#include "pages/itempage.h"
#include "backend/ratingshelper.h"
#include "backend/settings.h"
#include "widgets/gallery.h"
#include "widgets/stars.h"
#include <QScrollArea>
#include <DLabel>
#include <DNotifySender>

ItemPage::ItemPage(App *app)
{
    QScrollArea *scroll = new QScrollArea(this);
    QWidget *widget = new QWidget;
    scroll->setWidget(widget);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    widget->setLayout(layout);

    this->app = app;

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    mainLayout->addWidget(scroll);

    spinner->setFixedSize(50, 50);
    
    if (!app->fullData) {
        app->getFullData();
        spinner->start();
        layout->addWidget(spinner, 1, Qt::AlignCenter);
        connect(app->source, &Source::gotFullData, this, [ = ] (App *data) {
            if (data->id == app->id) {
                load();
                disconnect(app->source, &Source::gotFullData, this, nullptr);
            }
        });
    } else {
        load();
    }
}

void ItemPage::load()
{
    spinner->hide();
    layout->removeWidget(spinner);
    QHBoxLayout *header = new QHBoxLayout;
    header->setMargin(10);
    header->setAlignment(Qt::AlignVCenter);

    QLabel *icon = new QLabel;
    icon->setPixmap(app->icon.pixmap(app->icon.actualSize(QSize(64, 64))));
    header->addWidget(icon);

    QVBoxLayout *nameSection = new QVBoxLayout;
    nameSection->setMargin(0);
    nameSection->setAlignment(Qt::AlignTop);

    DLabel *name = new DLabel(app->name);
    QFont nameFont;
    nameFont.setPixelSize(22);
    name->setFont(nameFont);
    nameSection->addWidget(name);
    if (!app->developer.isNull()) {
        nameSection->addWidget(new DLabel(app->developer));
    }
    if (RatingsHelper::instance()->averageRating(app->id) != 0) {
        nameSection->addWidget(new stars(app->id));
    }
    header->addLayout(nameSection);

    header->addStretch();

    removeBtn = new DWarningButton;
    removeBtn->setText(tr("Uninstall"));
    removeBtn->hide();
    header->addWidget(removeBtn, 0, Qt::AlignVCenter);
    connect(removeBtn, &DWarningButton::clicked, this, [ = ] {
        app->uninstall();
    });

    header->addSpacing(10);
    installBtn = new DSuggestButton;
    installBtn->hide();
    header->addWidget(installBtn, 0, Qt::AlignVCenter);
    connect(installBtn, &DSuggestButton::clicked, this, [ = ] {
        if (app->state == App::Launchable)
            app->launch();
        else
            app->install();
    });

    layout->addLayout(header);

    progressBar = new DProgressBar;
    progressBar->hide();
    layout->addWidget(progressBar, 0, Qt::AlignTop);
    
    if (!app->screenshots.isEmpty()) {
        layout->addWidget(new gallery(app->screenshots));
    }

    DLabel *description = new DLabel;
    description->setWordWrap(true);
    description->setAlignment(Qt::AlignHCenter);
    description->setMargin(15);
    if (app->description.isNull()) {
        description->setText(QString("<i>%1</i>").arg(tr("No description provided.")));
    } else {
        description->setText(app->description);
    }

    layout->addWidget(description);

    connect(app->source, &Source::percentageChanged, this, [ = ] (App *data, int percentage) {
        if (data->id == app->id)
            progressBar->setValue(percentage);
    });
    connect(app->source, &Source::stateChanged, this, [ = ] (App *data) {
        if (data->id == app->id)
            updateButtons();
    });
    connect(app->source, &Source::installFinished, this, [ = ] (App *data) {
        if (data->id == app->id && settings::instance()->notifyInstall())
            DUtil::DNotifySender(tr("Installed \"%1\"").arg(app->name)).appIcon("dialog-ok").appName("DDE Store").timeOut(5000).call();
    });
    connect(app->source, &Source::uninstallFinished, this, [ = ] (App *data) {
        if (data->id == app->id && settings::instance()->notifyUninstall())
            DUtil::DNotifySender(tr("Uninstalled \"%1\"").arg(app->name)).appIcon("dialog-ok").appName("DDE Store").timeOut(5000).call();
    });

    updateButtons();
}

void ItemPage::updateButtons()
{
    switch (app->state)
    {
    case App::NotInstalled:
        removeBtn->hide();
        progressBar->hide();
        installBtn->show();
        installBtn->setDisabled(false);
        installBtn->setText(tr("Install (%1)").arg(QLocale().formattedDataSize(app->downloadSize)));
        break;
    case App::Launchable:
        progressBar->hide();
        installBtn->show();
        installBtn->setDisabled(false);
        installBtn->setText(tr("Open"));
        removeBtn->show();
        break;
    case App::Installed:
        progressBar->hide();
        installBtn->hide();
        removeBtn->show();
        break;
    case App::Installing:
        progressBar->show();
        progressBar->setValue(0);
        removeBtn->hide();
        installBtn->setDisabled(true);
        break;
    default:
        break;
    }
}
