#include "mainwindow.h"
#include "backend/sourcemanager.h"
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
        about.setVersion("1.2.2");

        MainWindow w;
        Dtk::Widget::moveToCenter(&w);
        w.show();

        if (!parser.positionalArguments().isEmpty()) {
            QUrl url(parser.positionalArguments()[0]);
            for (Source *source : SourceManager::instance()->sources()) {
                if (source->urlSchemes().contains(url.scheme()) && source->appFromUrl(url))
                    w.openItem(source->appFromUrl(url));
            }
        }

        return app.exec();
    }
    return 0;
}
