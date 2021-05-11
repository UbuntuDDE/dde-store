#include "pages/updatespage.h"
#include "backend/settings.h"
#include <QVBoxLayout>
#include <QTimer>
#include <DGuiApplicationHelper>
#include <DNotifySender>

UpdatesPage::UpdatesPage(MainWindow *parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);
    list = new List(tr("Updates"));
    list->setEmptyText(tr("Up to date"));

    systemUpdatesItem = tr("System Updates");
    systemUpdatesPopup = new DDialog(systemUpdatesItem, "");
    systemUpdatesPopup->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));

    mainwindow = parent;

    connect(list, &List::currentItemChanged, this, [ = ] (QVariant data) {
        if (data == systemUpdatesItem) {
            systemUpdatesPopup->exec();
        } else {
            for (App *app : appUpdates) {
                if (app->id == data.toString()) {
                    parent->openItem(app);
                    break;
                }
            }
        }
    });

    refreshButton = new DIconButton(this);
    connect(refreshButton, &DIconButton::clicked, this, [ = ] { refresh(true); });
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ] {
        if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
            refreshButton->setIcon(QIcon::fromTheme("refresh"));
        } else {
            refreshButton->setIcon(QIcon::fromTheme("refresh_dark"));
        }
    });
    refreshButton->setIcon(QIcon::fromTheme("refresh_dark"));
    refreshButton->setIconSize(QSize(20, 20));
    refreshButton->setDisabled(false);
    Q_EMIT(canRefresh());
    list->addHeaderWidget(refreshButton);

    updateButton = new DSuggestButton(tr("Update All"));
    updateButton->setDisabled(true);
    connect(updateButton, &DSuggestButton::clicked, this, [ = ] {
        updateButton->setDisabled(true);
        updateButton->setText(tr("Updating..."));
        refreshButton->setDisabled(true);
        Q_EMIT(cantRefresh());
        SourceManager::instance()->sources().first()->update();
    });
    list->addHeaderWidget(updateButton);

    for (Source *source : SourceManager::instance()->sources()) {
        connect(source, &Source::gotUpdates, this, [ = ] (QList<App*> updates) {
            for (App *app : updates) {
                if (app->source->name() == "PackageKit" && settings::instance()->nonApps() == 0 && !app->hasMetadata) {
                    if (systemUpdates.length() == 0)
                        list->addItem(systemUpdatesItem, QIcon::fromTheme("application-x-executable"));
                    systemUpdates << app;
                    systemUpdatesPopup->setMessage(systemUpdatesPopup->message() + "<br><b>" + app->package + "</b> - " + QLocale().formattedDataSize(app->downloadSize));
                } else {
                    appUpdates << app;
                    list->addItem(app->name, app->icon, app->id);
                }
            }
            if (SourceManager::instance()->sources().endsWith(source)) {
                list->load();
                updateButton->setDisabled(appUpdates.size() + systemUpdates.size() == 0);
                refreshButton->setDisabled(false);
                emit(canRefresh());
                mainwindow->setUpdateIndicatorVisible(appUpdates.size() + systemUpdates.size() > 0);
                qulonglong totalSize = 0;
                for (App *app : systemUpdates + appUpdates) {
                    totalSize += app->downloadSize;
                }
                updateButton->setText(tr("Update All (%1)").arg(QLocale().formattedDataSize(totalSize)));
                if (appUpdates.size() + systemUpdates.size() > 0 && settings::instance()->notifyAvailableUpdates())
                    DUtil::DNotifySender(tr("Updates Available")).appIcon("system-updated").appName("DDE Store").timeOut(10000).call();
            } else {
                SourceManager::instance()->sources()[SourceManager::instance()->sources().indexOf(source) + 1]->getUpdates();
            }
        });
        connect(source, &Source::updateFinished, this, [ = ] {
            if (SourceManager::instance()->sources().endsWith(source)) {
                refreshButton->setDisabled(false);
                refresh();
                if (settings::instance()->notifyFinishedUpdates())
                    DUtil::DNotifySender(tr("Updates Installed")).appIcon("system-updated").appName("DDE Store").timeOut(5000).call();
            } else {
                SourceManager::instance()->sources()[SourceManager::instance()->sources().indexOf(source) + 1]->update();
            }
        });
        connect(source, &Source::percentageChanged, this, [ = ] (App *app, int percentage) {
            if (appUpdates.contains(app))
                list->editItemText(app->id, QString::number(percentage) + "%");
            else if (systemUpdates.contains(app))
                list->editItemText(systemUpdatesItem, QString::number(percentage) + "%");
        });
    }

    layout->addWidget(list);

    if (settings::instance()->updateTime() != 1) {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [ = ] {
            if (appUpdates.size() + systemUpdates.size() == 0)
                refresh(true);
        });
        timer->start(3600000);
    }

    list->load();
    refresh();
}

void UpdatesPage::refresh(bool refreshCache)
{
    if (refreshButton->isEnabled()) {
        updateButton->setDisabled(true);
        refreshButton->setDisabled(true);
        Q_EMIT(cantRefresh());
        list->clear();
        list->unload();
        appUpdates.clear();
        systemUpdates.clear();
        systemUpdatesPopup->setMessage("");
        SourceManager::instance()->sources().first()->getUpdates();
    }
}
