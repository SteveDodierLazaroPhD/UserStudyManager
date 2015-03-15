#ifndef UCLNETQUERIES_H
#define UCLNETQUERIES_H

#include <QObject>

class UCLNetQueries : public QObject
{
    Q_OBJECT
public:
    explicit UCLNetQueries(QObject *parent = 0);
    ~UCLNetQueries();

signals:

public slots:
};

#endif // UCLNETQUERIES_H
