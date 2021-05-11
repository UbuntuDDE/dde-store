#ifndef SOURCE_H
#define SOURCE_H

#include <QObject>
#include <QIcon>

struct App;

class Source : public QObject
{
    Q_OBJECT

public:
    virtual QString name() = 0;
    virtual void getInstalled() = 0;
    virtual void getUpdates(bool refreshCache = false) = 0;
    virtual void getFullData(App *app) = 0;
    virtual void install(App *app) = 0;
    virtual void uninstall(App *app) = 0;
    virtual void launch(App *app) = 0;
    virtual void search(QString query) = 0;
    virtual void update() = 0;
    virtual QStringList urlSchemes() = 0;
    virtual App *appFromUrl(QUrl url) = 0;
    void error(QString text);
    bool preventingClose = false;

signals:
    void gotInstalled(QList<App*> installed);
    void gotUpdates(QList<App*> updates);
    void gotFullData(App *app);
    void percentageChanged(App *app, int percentage);
    void installFinished(App *app);
    void uninstallFinished(App *app);
    void searchFinished(QList<App*> results);
    void updateFinished();
    void stateChanged(App *app);

};

Q_DECLARE_INTERFACE(Source, "org.dekzi.dde-store.SourceInterface")

struct App
{
    Source *source;
    QString id;
    QString package;
    QString name;
    QIcon icon;
    QString description;
    QString developer;
    QList<QUrl> screenshots;
    int ratings;
    qulonglong downloadSize;
    bool fullData = false;
    bool hasMetadata;
    enum State { Installed, Installing, NotInstalled, Launchable } state;
    void install() { source->install(this); };
    void uninstall() { source->uninstall(this); };
    void launch() { source->launch(this); };
    void getFullData() { source->getFullData(this); };
};

class SourceManager : public QObject
{
    Q_OBJECT

public:
    static SourceManager *instance();
    QList<Source*> sources();
    Source *getSource(QString name);
    bool preventingClose();

private:
    SourceManager();
    static SourceManager *currentInstance;
    QList<Source*> m_sources;

};

#endif // SOURCE_H