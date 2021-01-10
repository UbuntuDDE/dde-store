#include "mainwindow.h"
#include <DApplication>
#include <DAboutDialog>
#include <DWidgetUtil>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication app(argc, argv);
    if (app.setSingleInstance("dde-store")) {
        app.loadTranslator();
        app.setApplicationName("dde-store");
        app.setOrganizationName("dekzi");
        app.setAttribute(Qt::AA_UseHighDpiPixmaps);
        DAboutDialog about;
        app.setAboutDialog(&about);
        about.setProductIcon(QIcon::fromTheme("deepin-app-store"));
        about.setProductName("DDE Store");
        about.setDescription(qApp->translate("main", "An app store for DDE built with DTK") + "<br/>" + qApp->translate("main", "Created by %1").arg("<a href='https://github.com/dekzi'>dekzi</a>") + "<br/><br/><a href='https://github.com/dekzi/dde-store'>https://github.com/dekzi/dde-store</a>");
        about.setVersion("1.2.0");
        MainWindow w;
        Dtk::Widget::moveToCenter(&w);
        w.show();
        return app.exec();
    }
    return 0;
}
