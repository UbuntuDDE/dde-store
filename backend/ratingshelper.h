#ifndef RATINGSHELPER_H
#define RATINGSHELPER_H

#include <QObject>
#include <QHash>
#include <QJsonObject>

class RatingsHelper : public QObject
{
    Q_OBJECT

public:
    static RatingsHelper *instance();
    double averageRating(QString app);
    int totalRatings(QString app);

private:
    static RatingsHelper *currentInstance;
    RatingsHelper();
    QHash<QString, QJsonObject> ratingsList;

signals:
    void fetched();
};

#endif // RATINGSHELPER_H