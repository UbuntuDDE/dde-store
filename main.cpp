#include "mainwindow.h"
#include "backend/appstreamhelper.h"
#include <DApplication>
#include <DAboutDialog>
#include <DWidgetUtil>
#include <QDebug>
#include <QUrl>
#include <QCommandLineParser>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("url", "Appstream url of an app to open");
    parser.process(app);

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
        about.setVersion("1.2.1");

        MainWindow w;
        Dtk::Widget::moveToCenter(&w);
        w.show();

        if (!parser.positionalArguments().isEmpty()) {
            QUrl url(parser.positionalArguments()[0]);
            if (url.scheme() == "appstream") {
                // Converting to "local file" so double backslashes in the url work properly
                QString id(QUrl::fromLocalFile(url.toString()).fileName());
                QString shortId;
                if (id.endsWith(".desktop")) {
                    shortId = id.left(id.lastIndexOf("."));
                }

                if (!AppStreamHelper::instance()->packageFromID(id).isEmpty()) {
                    w.openItem(AppStreamHelper::instance()->packageFromID(id), id);
                } else if (!AppStreamHelper::instance()->packageFromID(shortId).isEmpty()) {
                    w.openItem(AppStreamHelper::instance()->packageFromID(shortId), shortId);
                } else {
                    qDebug() << "[ ERROR ] Unable to open app from url" << parser.positionalArguments()[0];
                }
            }
        }

        return app.exec();
    }
    return 0;
}
