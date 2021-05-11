#ifndef ITEMPAGE_H
#define ITEMPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <DSuggestButton>
#include <DWarningButton>
#include <DProgressBar>
#include <DSpinner>
#include "backend/sourcemanager.h"

DWIDGET_USE_NAMESPACE

class ItemPage : public QWidget
{
    Q_OBJECT

public:
    ItemPage(App *app);
    void load();

private:
    QVBoxLayout *layout;
    DWarningButton *removeBtn;
    DSuggestButton *installBtn;
    DProgressBar *progressBar;
    DSpinner *spinner = new DSpinner;
    App *app;
    void updateButtons();
};

#endif // ITEMPAGE_H