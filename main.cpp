#include "mainwindow.h"
#include <DApplication>
#include <DAboutDialog>
#include <DWidgetUtil>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication a(argc, argv);
    a.loadTranslator();
    a.setApplicationName("dde-store");
    a.setOrganizationName("dekzi");
    DAboutDialog about;
    a.setAboutDialog(&about);
    about.setProductIcon(QIcon::fromTheme("deepin-app-store"));
    about.setProductName("DDE Store");
    about.setDescription(qApp->translate("main", "An app store for DDE built with DTK") + "<br/>" + qApp->translate("main", "Created by %1").arg("<a href='https://github.com/dekzi'>Dekzi</a>") + "<br/><br/><a href='https://github.com/dekzi/dde-store'>https://github.com/dekzi/dde-store</a>");
    about.setVersion("1.0.3");
    MainWindow *w = new MainWindow;
    Dtk::Widget::moveToCenter(w);
    w->show();
    return a.exec();
}
