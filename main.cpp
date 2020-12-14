#include "mainwindow.h"
#include <DApplication>
#include <DAboutDialog>
#include <DWidgetUtil>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();
    DApplication *app = new DApplication(argc, argv);
    app->loadTranslator();
    app->setApplicationName("dde-store");
    app->setOrganizationName("dekzi");
    DAboutDialog about;
    app->setAboutDialog(&about);
    about.setProductIcon(QIcon::fromTheme("deepin-app-store"));
    about.setProductName("DDE Store");
    about.setDescription(qApp->translate("main", "An app store for DDE built with DTK") + "<br/>" + qApp->translate("main", "Created by %1").arg("<a href='https://github.com/dekzi'>Dekzi</a>") + "<br/><br/><a href='https://github.com/dekzi/dde-store'>https://github.com/dekzi/dde-store</a>");
    about.setVersion("1.1.0");
    MainWindow *window = new MainWindow;
    Dtk::Widget::moveToCenter(window);
    window->show();
    return app->exec();
}
