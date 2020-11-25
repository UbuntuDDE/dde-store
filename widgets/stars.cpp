#include "widgets/stars.h"
#include "backend/ratingshelper.h"
#include <QHBoxLayout>
#include <QLabel>

stars::stars(QString app)
{
    QHBoxLayout *layout = new QHBoxLayout;
    this->setLayout(layout);
    layout->setMargin(0);
    layout->setAlignment(Qt::AlignLeft);
    double rating = RatingsHelper::instance()->averageRating(app);
    for (int i = 0; i < int(rating); i++) {
        QLabel *star = new QLabel;
        star->setPixmap(QPixmap("://resources/icons/star.png"));
        layout->addWidget(star);
    }
    if (QString::number(rating).endsWith(".5")) {
        QLabel *halfstar = new QLabel;
        halfstar->setPixmap(QPixmap("://resources/icons/starhalf.png"));
        layout->addWidget(halfstar);
    }
    QLabel *total = new QLabel(tr("(%1 ratings)").arg(RatingsHelper::instance()->totalRatings(app)));
    layout->addWidget(total);
}