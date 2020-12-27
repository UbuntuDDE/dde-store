#include "mainwindow.h"
#include "pages/homepage.h"
#include "pages/updatespage.h"
#include "backend/settings.h"
#include "backend/packagekithelper.h"
#include "pages/categorypage.h"
#include <DTitlebar>
#include <DSearchEdit>
#include <DSettingsDialog>
#include <DDialog>
#include <DMenu>
#include <DCheckBox>
#include <DRadioButton>
#include <QHBoxLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    this->setMinimumSize(950, 600);
    QWidget *centralWidget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Initialize updates page here so tray button works

    UpdatesPage *updates = new UpdatesPage(this);
    updatesPage = updates;

    // Initialize tray item and buttons

    DMenu *trayIconMenu = new DMenu;
    QAction *checkUpdatesAction = new QAction(tr("Check for updates"));
    QAction *quitAction = new QAction(tr("Quit"));
    trayIconMenu->addActions({checkUpdatesAction, quitAction});
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip("DDE Store");

    // Set up button actions

    connect(quitAction, &QAction::triggered, this, &MainWindow::close);

    connect(updates, &UpdatesPage::cantRefresh, this, [ = ] {
        checkUpdatesAction->setDisabled(true);
    });

    connect(updates, &UpdatesPage::canRefresh, this, [ = ] {
        checkUpdatesAction->setDisabled(false);
    });

    connect(checkUpdatesAction, &QAction::triggered, updates, [ = ] { updates->refresh(true); });

    connect(trayIcon, &QSystemTrayIcon::activated, this, [ = ] {
        show();
        trayIcon->hide();
    });

    // Initialize sidebar (navView) and titlebar

    initTitlebar();
    initNav();

    layout->addWidget(navView, 1);
    layout->addWidget(stackedWidget, 7);
}

void MainWindow::initTitlebar()
{
    DTitlebar *titlebar = this->titlebar();
    titlebar->setIcon(QIcon::fromTheme("deepin-app-store"));

    // Create back button

    backButton = new DButtonBoxButton(DStyle::SP_ArrowLeave);
    backButton->setDisabled(true);
    backButton->setFixedSize(36, 36);
    connect(backButton, &DButtonBoxButton::clicked, this, [ = ] {
        pageHistoryIndex -= 1;
        buttonNavigated = true;
        stackedWidget->setCurrentWidget(pageHistory[pageHistoryIndex]);
        // If the new current page has an entry in the sidebar then select that entry
        if (pageHistoryIndex < navModel->rowCount()) {
            navView->setCurrentIndex(navModel->index(stackedWidget->indexOf(pageHistory[pageHistoryIndex]), 0));
        }
    });

    // Create forward button

    forwardButton = new DButtonBoxButton(DStyle::SP_ArrowEnter);
    forwardButton->setDisabled(true);
    forwardButton->setFixedSize(36, 36);
    connect(forwardButton, &DButtonBoxButton::clicked, this, [ = ] {
        // The same as back button, just increases the index rather than decreasing it
        pageHistoryIndex += 1;
        buttonNavigated = true;
        stackedWidget->setCurrentWidget(pageHistory[pageHistoryIndex]);
        if (pageHistoryIndex < navModel->rowCount()) {
            navView->setCurrentIndex(navModel->index(stackedWidget->indexOf(pageHistory[pageHistoryIndex]), 0));
        }
    });

    // Initialize the button box and add the buttons

    DButtonBox *buttonBox = new DButtonBox();
    buttonBox->setButtonList({backButton, forwardButton}, false);
    buttonBox->setFocusPolicy(Qt::NoFocus);
    titlebar->addWidget(buttonBox, Qt::AlignLeft);

    // Create search box

    DSearchEdit *searchBox = new DSearchEdit();
    searchBox->setFixedWidth(300);
    titlebar->addWidget(searchBox);
    connect(searchBox, &DSearchEdit::returnPressed, this, [ = ] {
        // Create a new category page for the search
        CategoryPage *page = new CategoryPage(this, QString("\"%1\"").arg(searchBox->text()), searchBox->text());
        // Open it
        stackedWidget->addWidget(page);
        stackedWidget->setCurrentWidget(page);
        // Clear the sidebar selection
        navView->setCurrentIndex(QModelIndex());
        navView->clearSelection();
    });
    connect(searchBox, &DSearchEdit::searchAborted, this, [ = ] {
        // Go to homepage when search aborted
        navView->setCurrentIndex(navModel->index(0, 0));
        stackedWidget->setCurrentIndex(0);
    });

    titlebar->setMenu(new QMenu(titlebar));
    QAction *settingsAction = new QAction(tr("Settings"), this);
    titlebar->menu()->addAction(settingsAction);

    connect(titlebar->menu(), &QMenu::triggered, this, [ = ] (QAction *action) {
        if (action == settingsAction) {
            DSettingsDialog *dialog = new DSettingsDialog;
            dialog->updateSettings(settings::instance()->appsettings);
            dialog->exec();
        }
    });
}

void MainWindow::initNav()
{
    // Set up list
    navView->setViewportMargins(QMargins(10, 10, 10, 10));
    navView->setMinimumWidth(188);
    navView->setSpacing(0);
    navView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    navView->setIconSize(QSize(28, 28));
    navView->setItemSize(QSize(168, 56));
    navView->setAutoFillBackground(true);
    navView->setModel(navModel);
    navView->setEditTriggers(QListView::NoEditTriggers);

    // Add entries

    addPage(tr("Home"), "home.svg", new HomePage(this));
    addPage(tr("Messaging"), "chat.svg", new CategoryPage(this, tr("Messaging"), "InstantMessaging"));
    addPage(tr("Internet"), "internet.svg", new CategoryPage(this, tr("Internet"), "Network"));
    addPage(tr("Games"), "games.svg", new CategoryPage(this, tr("Games"), "Game"));
    addPage(tr("Development"), "development.svg", new CategoryPage(this, tr("Development"), "Development"));
    addPage(tr("Office"), "office.svg", new CategoryPage(this, tr("Office"), "Office"));
    addPage(tr("Graphics"), "graphics.svg", new CategoryPage(this, tr("Graphics"), "Graphics"));
    addPage(tr("Video"), "video.svg", new CategoryPage(this, tr("Video"), "Video"));
    addPage(tr("Music"), "music.svg", new CategoryPage(this, tr("Music"), "Music"));
    addPage(tr("System"), "system.svg", new CategoryPage(this, tr("System"), "System"));
    addPage(tr("Installed"), "installed.svg", new CategoryPage(this, tr("Installed"), "Installed"));
    addPage("Updates", "updates.svg", updatesPage);

    // When the current entry is changed
    connect(navView, qOverload<const QModelIndex &>(&DListView::currentChanged), this, [ = ] (const QModelIndex &previous) {
        // If it isn't -1 (-1 means no sidebar entries are selected)
        if (navView->currentIndex().row() != -1) {
            // If theme is light, set the selected icon to active and the previous one to normal
            if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
                auto currentItem = navModel->itemFromIndex(navView->currentIndex());
                currentItem->setIcon(QIcon("://resources/icons/active/" + pageIcons.value(currentItem->text())));
                if (previous.row() != -1) {
                    auto previousItem = navModel->itemFromIndex(previous);
                    previousItem->setIcon(QIcon("://resources/icons/light/" + pageIcons.value(previousItem->text())));
                }
            }
            // Open the entrie's widget
            stackedWidget->setCurrentIndex(navView->currentIndex().row());
        } else {
            // If it is -1 and there is an active sidebar entry, set it to normal
            if (previous.row() != -1) {
                auto selectedItem = navModel->itemFromIndex(previous);
                if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
                    selectedItem->setIcon(QIcon("://resources/icons/light/" + pageIcons.value(selectedItem->text())));
                }
            }
        }
                
        if (!buttonNavigated) {
            // Add the page to history and increase index by 1
            pageHistory << stackedWidget->currentWidget();
            pageHistoryIndex += 1;
            // If the index isn't at the end of the history (the user pressed back)
            if (pageHistoryIndex != pageHistory.length() - 1) {
                // Remove all entires after this current one
                for (int i = 0; i < pageHistory.length() - pageHistoryIndex + 1; i++) {
                    pageHistory.removeLast();
                }
                pageHistory << stackedWidget->currentWidget();
                pageHistoryIndex = pageHistory.length() - 1;
            }
        } else {
            buttonNavigated = false;
        }

        if (pageHistoryIndex < 1) {
            backButton->setDisabled(true);
        } else {
            backButton->setDisabled(false);
        }

        if (pageHistoryIndex != pageHistory.length() - 1) {
            forwardButton->setDisabled(false);
        } else {
            forwardButton->setDisabled(true);
        }
    });

    // Select homepage by default
    navView->setCurrentIndex(navModel->index(0, 0));

    // Set icon type based on current theme
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ] {
        for (int i = 0; i < navModel->rowCount(); i++) {
            QIcon icon;
            QString iconname = pageIcons.value(navModel->item(i)->text());
            auto theme = DGuiApplicationHelper::instance()->themeType();
            if (theme == DGuiApplicationHelper::LightType) {
                if (navView->currentIndex().row() == i) {
                    icon = QIcon("://resources/icons/active/" + iconname);
                } else {
                    icon = QIcon("://resources/icons/light/" + iconname);
                }
            } else {
                icon = QIcon("://resources/icons/dark/" + iconname);
            }
            navModel->item(i)->setIcon(icon);
        }
    });
}

void MainWindow::addPage(QString name, QString iconname, QWidget *widget)
{
    DStandardItem *item = new DStandardItem(name);
    navModel->appendRow(item);
    pageIcons[name] = iconname;
    stackedWidget->addWidget(widget);
    if (iconname == "updates.svg") {
        updateIndicator = new DViewItemAction(Qt::AlignVCenter, QSize(16, 16), QSize(16, 16), false);
        updateIndicator->setIcon(QIcon("://resources/icons/indicator.svg"));
        updateIndicator->setVisible(false);
        item->setActionList(Qt::Edge::RightEdge, {updateIndicator});
    }
}

void MainWindow::openItem(QString app, QString id, bool snap)
{
    // If the item page is in the list
    if (itemPageList.values(id).length() > 0) {
        // Open it
        stackedWidget->setCurrentWidget(itemPageList.value(id));
    } else {
        // If not, create the page and add it to the list
        ItemPage *widget = new ItemPage(app, snap);
        stackedWidget->addWidget(widget);
        stackedWidget->setCurrentWidget(widget);
        itemPageList.insert(id, widget);
    }
    navView->setCurrentIndex(QModelIndex());
    navView->clearSelection();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!trayIcon->isVisible()) {
        if (!settings::instance()->remembered()) {
            DDialog dialog;
            DRadioButton *exitButton = new DRadioButton(tr("Exit"));
            exitButton->setChecked(!settings::instance()->tray());
            DRadioButton *minimizeButton = new DRadioButton(tr("Minimize to system tray"));
            minimizeButton->setChecked(settings::instance()->tray());
            DCheckBox *rememberBox = new DCheckBox(tr("Do not ask again"));

            dialog.setTitle(tr("Please choose your action"));
            dialog.setIcon(QIcon::fromTheme("deepin-app-store"));
            dialog.addContent(exitButton);
            dialog.addContent(minimizeButton);
            dialog.addContent(rememberBox);
            dialog.addButton(tr("Cancel"), false, DDialog::ButtonNormal);
            dialog.addButton(tr("Confirm"), true, DDialog::ButtonRecommend);
            int index = dialog.exec();

            if (index == 1) {
                if (rememberBox->isChecked()) {
                    settings::instance()->setValue("basic.behaviour.remember", true);
                }

                if (exitButton->isChecked()) {
                    settings::instance()->setValue("basic.behaviour.tray", false);
                } else if (minimizeButton->isChecked()) {
                    settings::instance()->setValue("basic.behaviour.tray", true);
                    event->ignore();
                    trayIcon->show();
                    hide();
                }
            } else {
                event->ignore();
            }
        } else {
            if (settings::instance()->tray()) {
                event->ignore();
                trayIcon->show();
                hide();
            }
        }
    }

    if (PackageKitHelper::instance()->preventClose) {
        DDialog dialog;
        dialog.setIcon(style()->standardIcon(QStyle::SP_MessageBoxCritical));
        dialog.setTitle(tr("Cannot close while app is being installed"));
        dialog.addButton(tr("OK"));
        dialog.exec();
        event->ignore();
    }
}

void MainWindow::setUpdateIndicatorVisible(bool value)
{
    updateIndicator->setVisible(value);
}
