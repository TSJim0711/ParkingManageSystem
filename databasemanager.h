#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#define dbLoc "parkn.db"

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDateTime>

class databaseManager
{
public:
    databaseManager();
    ~databaseManager();

    struct eventRtnKit
    {
        enum{fail, carIn, carOut} dir;
        QDateTime carInT;
        QDateTime carOutT;
        int payPrice;
    };
    eventRtnKit vehiScanned(QString plateNo);

    int pricePerUnit = 8;//$8 every 5 sec
    int unitInSec = 5;

private:
    QSqlDatabase db;
    QSqlQueryModel *dbModel;

    bool vehiInBound(QString plateNo);
    eventRtnKit vehiOutBound(QString plateNo);


};

#endif // DATABASEMANAGER_H
