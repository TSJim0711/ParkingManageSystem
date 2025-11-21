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

    int pricePerUnit = 0.1;//$0.1 every 1 sec. 1hr pay $360, good price
    int unitInSec = 1;
    QSqlQueryModel *openSQLResult;

    struct eventRtnKit
    {
        enum{fail, carIn, carOut} dir;
        QDateTime carInT;
        QDateTime carOutT;
        int payPrice;
    };
    eventRtnKit vehiScanned(QString plateNo);
    QSqlQueryModel *execSQLSelect(QString sqlCmd);

private:
    QSqlDatabase db;
    bool vehiInBound(QString plateNo);
    eventRtnKit vehiOutBound(QString plateNo);
};

#endif // DATABASEMANAGER_H
