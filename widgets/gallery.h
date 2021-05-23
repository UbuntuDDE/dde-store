#ifndef GALLERY_H
#define GALLERY_H

#include <QWidget>
#include <QLabel>
#include <DIconButton>
#include <DSpinner>
#include <DPageIndicator>
#include <QNetworkAccessManager>
#include <QEvent>
#include "mainwindow.h"
#include "backend/sourcemanager.h"

DWIDGET_USE_NAMESPACE

class gallery : public QWidget
{
    Q_OBJECT

public:
    gallery(QList<QUrl> imageList);
    gallery(QList<QPair<QString, App*>> imageList, MainWindow *parent);

private:
    QLabel *imageView;
    int currentImage;
    void init();
    void changeImage(int index);
    void changeLocalImage(int index);
    QList<QUrl> images;
    QList<QPair<QString, App*>> banners;
    DIconButton *backButton;
    DIconButton *forwardButton;
    DSpinner *spinner;
    DPageIndicator *pageIndicator;
    MainWindow *mainwindow;
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // GALLERY_H