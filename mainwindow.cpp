#include "mainwindow.h"
#include "pages/homepage.h"
#include "pages/updatespage.h"
#include "backend/settings.h"
#include "backend/packagekithelper.h"
#include "backend/ratingshelper.h"
#include "pages/categorypage.h"
#include <DTitlebar>
#include <DSearchEdit>
#include <DSettingsDialog>
#include <DDialog>
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
    
    initTitlebar();
    initNav();

    RatingsHelper::instance();

    layout->addWidget(navView, 1);
    layout->addWidget(stackedWidget, 7);
}

void MainWindow::initTitlebar()
{
    DTitlebar *titlebar = this->titlebar();
    titlebar->setIcon(QIcon::fromTheme("deepin-app-store"));

    backButton = new DButtonBoxButton(DStyle::SP_ArrowLeave);
    backButton->setDisabled(true);
    backButton->setFixedSize(36, 36);
    connect(backButton, &DButtonBoxButton::clicked, this, [ = ] {
        pageHistoryIndex -= 1;
        buttonNavigated = true;
        stackedWidget->setCurrentWidget(pageHistory[pageHistoryIndex]);
        if (pageHistory[pageHistoryIndex]->objectName() != "ItemPage") {
            navView->setCurrentIndex(navModel->index(stackedWidget->indexOf(pageHistory[pageHistoryIndex]), 0));
        }
    });

    forwardButton = new DButtonBoxButton(DStyle::SP_ArrowEnter);
    forwardButton->setDisabled(true);
    forwardButton->setFixedSize(36, 36);
    connect(forwardButton, &DButtonBoxButton::clicked, this, [ = ] {
        pageHistoryIndex += 1;
        buttonNavigated = true;
        stackedWidget->setCurrentWidget(pageHistory[pageHistoryIndex]);
        if (pageHistory[pageHistoryIndex]->objectName() != "ItemPage") {
            navView->setCurrentIndex(navModel->index(stackedWidget->indexOf(pageHistory[pageHistoryIndex]), 0));
        }
    });

    QList<DButtonBoxButton *> buttonList;
    buttonList << backButton << forwardButton;

    DButtonBox *buttonBox = new DButtonBox();
    buttonBox->setButtonList(buttonList, false);
    buttonBox->setFocusPolicy(Qt::NoFocus);
    titlebar->addWidget(buttonBox, Qt::AlignLeft);

    DSearchEdit *searchBox = new DSearchEdit();
    searchBox->setFixedWidth(300);
    titlebar->addWidget(searchBox);
    connect(searchBox, &DSearchEdit::returnPressed, this, [ = ] {
        CategoryPage *page = new CategoryPage(this, QString("\"%1\"").arg(searchBox->text()), searchBox->text());
        stackedWidget->addWidget(page);
        stackedWidget->setCurrentWidget(page);
        navView->setCurrentIndex(QModelIndex());
        navView->clearSelection();
    });
    connect(searchBox, &DSearchEdit::searchAborted, this, [ = ] {
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
    navView->setViewportMargins(QMargins(10, 10, 10, 10));
    navView->setMinimumWidth(188);
    navView->setSpacing(0);
    navView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    navView->setIconSize(QSize(28, 28));
    navView->setItemSize(QSize(168, 56));
    navView->setAutoFillBackground(true);
    navView->setModel(navModel);
    navView->setEditTriggers(QListView::NoEditTriggers);

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
    addPage("Updates", "updates.svg", new UpdatesPage(this));

    connect(navView, qOverload<const QModelIndex &>(&DListView::currentChanged), this, [ = ] (const QModelIndex &previous) {
        if (navView->currentIndex().row() != -1) {
            if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
                auto currentItem = navModel->itemFromIndex(navView->currentIndex());
                currentItem->setIcon(QIcon("://resources/icons/active/" + pageIcons.value(currentItem->text())));
                if (previous.row() != -1) {
                    auto previousItem = navModel->itemFromIndex(previous);
                    previousItem->setIcon(QIcon("://resources/icons/light/" + pageIcons.value(previousItem->text())));
                }
            }
            stackedWidget->setCurrentIndex(navView->currentIndex().row());
        } else {
            if (previous.row() != -1) {
                auto selectedItem = navModel->itemFromIndex(previous);
                if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
                    selectedItem->setIcon(QIcon("://resources/icons/light/" + pageIcons.value(selectedItem->text())));
                }
            }
        }
                
        if (!buttonNavigated) {
            pageHistory << stackedWidget->currentWidget();
            pageHistoryIndex += 1;
            if (pageHistoryIndex != pageHistory.length() - 1) {
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

    navView->setCurrentIndex(navModel->index(0, 0));

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

void MainWindow::openItem(QString app)
{
    if (itemPageList.values(app).length() > 0) {
        stackedWidget->setCurrentWidget(itemPageList.value(app));
    } else {
        ItemPage *widget = new ItemPage(app);
        stackedWidget->addWidget(widget);
        stackedWidget->setCurrentWidget(widget);
        itemPageList.insert(app, widget);
    }
    navView->setCurrentIndex(QModelIndex());
    navView->clearSelection();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
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