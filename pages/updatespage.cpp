#include "pages/updatespage.h"
#include "backend/packagekithelper.h"
#include "backend/appstreamhelper.h"
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
    mainwindow = parent;

    systemUpdatesPopup = new DDialog;
    systemUpdatesPopup->setTitle(tr("System Updates"));
    systemUpdatesPopup->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
    connect(list, &List::currentItemChanged, this, [ = ] (QVariant data) {
        if (data.toString() == "sysupdates") {
            systemUpdatesPopup->exec();
        } else {
            parent->openItem(data.toString());
        }
    });

    refreshButton = new DIconButton(this);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ] {
        if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
            refreshButton->setIcon(QIcon::fromTheme("refresh"));
        } else {
            refreshButton->setIcon(QIcon::fromTheme("refresh_dark"));
        }
    });
    refreshButton->setIconSize(QSize(20, 20));
    refreshButton->setDisabled(true);
    Q_EMIT(cantRefresh());
    list->addHeaderWidget(refreshButton);

    updateButton = new DSuggestButton(tr("Update All"));
    updateButton->setDisabled(true);
    list->addHeaderWidget(updateButton);

    connect(updateButton, &DSuggestButton::clicked, this, [ = ] {
        updateButton->setDisabled(true);
        updateButton->setText(tr("Updating..."));
        refreshButton->setDisabled(true);
        Q_EMIT(cantRefresh());
        PackageKitHelper::instance()->update(this);
    });

    connect(refreshButton, &DIconButton::clicked, this, [ = ] { refresh(true); });

    updateButton->setText(tr("Update All (%1)").arg(QLocale().formattedDataSize(totalSize)));
    updateButton->setDisabled(false);
    refreshButton->setDisabled(false);
    Q_EMIT(canRefresh());

    PackageKitHelper::instance()->getUpdates(this);

    layout->addWidget(list);

    if (settings::instance()->updateTime() != 1) {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [ = ] {
            if (!updates.isEmpty()) {
                refresh(true);
            }
        });
        timer->start(3600000);
    }
}

void UpdatesPage::insertSystemUpdate(QString package, int size)
{
    systemUpdatesPopup->setMessage(systemUpdatesPopup->message().append(
        "<br><b>" + PackageKitHelper::instance()->nameFromID(package) + "</b> - " + QLocale().formattedDataSize(size))
    );
    updates << package;
}

void UpdatesPage::insertItem(QString name, QIcon icon, QString id, int size)
{
    list->addItem(name, icon, id);
    totalSize += size;
    appUpdates << id;
    updates << id;
}

void UpdatesPage::load()
{
    if (!updates.isEmpty()) {
        list->addItem(tr("System Updates"), QIcon::fromTheme("application-x-executable"), "sysupdates");
        if (settings::instance()->notifyAvailableUpdates()) {
            Dtk::Core::DUtil::DNotifySender(tr("Updates Available")).appIcon("system-updated").appName("DDE Store").timeOut(10000).call();
        }
    }
    list->load();
}

void UpdatesPage::updatePercent(QString package, uint percent)
{
    if (appUpdates.contains(package)) {
        list->editItemText(package, QString::number(percent) + "%");
    } else if (appUpdates.contains(package)) {
        list->editItemText(package, QString::number(percent) + "%");
    } else {
        list->editItemText("sysupdates", QString::number(percent) + "%");
    }
}

void UpdatesPage::refresh(bool refreshCache)
{
    if (refreshButton->isEnabled()) {
        updateButton->setDisabled(true);
        refreshButton->setDisabled(true);
        Q_EMIT(cantRefresh());
        list->clear();
        list->unload();
        PackageKitHelper::instance()->getUpdates(this, refreshCache);
    }
}
