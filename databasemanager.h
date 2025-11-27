#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#define dbLoc "parkn.db"

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDateTime>
#include <QRandomGenerator>

class databaseManager
{
public:
    databaseManager();
    ~databaseManager();

    float pricePerUnit = 0.2;//$0.1 every 1 sec. 1hr pay $360, good price
    int unitInSec = 1;
    int carParkSpace=50;
    QSqlQueryModel *openSQLResult;

    struct eventRtnKit
    {
        enum{fail, carIn, carOut} dir;
        QDateTime carInT;
        QDateTime carOutT;
        int stayTime;
        QString tradeID;
        float payPrice;
    };
    eventRtnKit vehiScanned(QString plateNo);
    QSqlQueryModel *execSQLSelect(QString sqlCmd);
    void clientPaid(QString paymentID);
    int getParkingCarCount();

private:
    QSqlDatabase db;
    bool vehiInBound(QString plateNo);
    eventRtnKit vehiOutBound(QString plateNo);
    QString tradeIdPossiChar="0123456789";
};

#endif // DATABASEMANAGER_H
