#ifndef ITEMPAGE_H
#define ITEMPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <DSuggestButton>
#include <DWarningButton>
#include <DProgressBar>
#include "backend/appstreamhelper.h"

DWIDGET_USE_NAMESPACE

class ItemPage : public QWidget
{
    Q_OBJECT

public:
    ItemPage(QString app, bool snap);
    void setData(AppStreamHelper::appData data);
    void setInstallButton(QString packageId, QString type, QString param = nullptr);
private:
    QVBoxLayout *layout;
    DWarningButton *removeBtn;
    DSuggestButton *installBtn;
    DProgressBar *progressBar;
    bool isSnap;
};

#endif // ITEMPAGE_H