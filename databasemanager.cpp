#include "databasemanager.h"
#include <QSqlError>
#include <QRandomGenerator>//auto seeding1

databaseManager::databaseManager()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("1.db");
    db.open();
    QSqlQuery query(db);
    bool queryValid = query.exec("CREATE TABLE IF NOT EXISTS carPresent (plateNo TEXT NOT NULL, carInTime INT NOT NULL, PRIMARY KEY (plateNo))");//unix time
    if(queryValid==false)
        qDebug()<<"SQLErr: Create carPresent fail when needed.";
    queryValid = query.exec("CREATE TABLE IF NOT EXISTS carLog (paymentID INT UNIQUE NOT NULL, plateNo TEXT NOT NULL, carInTime INT NOT NULL, carOutTime INT, PRIMARY KEY (paymentID))");
    if(queryValid==false)
        qDebug()<<"SQLErr: Create carLog fail when needed.";
    queryValid = query.exec("CREATE TABLE IF NOT EXISTS payment (paymentID INT UNIQUE NOT NULL, amount DECIMAL(10,2) NOT NULL, paidFlag BOOL NOT NULL, PRIMARY KEY (paymentID))");//amount 10 整数 2小数
    if(queryValid==false)
        qDebug()<<"SQLErr: Create payment fail when needed.";
    openSQLResult = new QSqlQueryModel();
}
databaseManager::~databaseManager()
{
    if (db.isOpen())
        db.close();
    free(openSQLResult);
}

databaseManager::eventRtnKit databaseManager::vehiScanned(QString plateNo)
{
    eventRtnKit rtnKit;
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM carPresent WHERE plateNo=:plateNo");
    query.bindValue(":plateNo", plateNo);
    query.finish();
    if(!query.exec())
    {
        qDebug()<<"SQLErr: vehiScanned check vehi in DB failed: "<<query.lastError().driverText();
        rtnKit.dir=eventRtnKit::fail;
        return rtnKit;
    }
    query.next();
    if(query.value(0).toInt()==0)//not in car park, car getting in
    {
        vehiInBound(plateNo);
        rtnKit.dir=eventRtnKit::carIn;
    }
    else//car getting out
    {
        qDebug()<<"Size: "<<query.value(0).toInt();
        rtnKit = vehiOutBound(plateNo);
    }
    return rtnKit;
}

bool databaseManager::vehiInBound(QString plateNo)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO carPresent (plateNo, carInTime) VALUES(:plateNo, :curTime)");//push into db
    query.bindValue(":plateNo",plateNo);
    query.bindValue(":curTime",QDateTime::currentSecsSinceEpoch());
    query.exec();
    if(query.isValid()==false)
    {
        qDebug()<<"SQLErr: vehiInBound insert vehi to DB failed."<<query.lastError().databaseText();
        return 0;
    }
    return 1;
}

databaseManager::eventRtnKit databaseManager::vehiOutBound(QString plateNo)
{
    eventRtnKit rtnKit;
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM carPresent WHERE plateNo=:plateNo");//car record with enrty log and no exit log
    query.bindValue(":plateNo",plateNo);
    query.finish();query.exec();query.next();
    if(query.isValid()==false)
    {
        qDebug()<<"SQLErr: vehiOutBound update vehi to DB failed.";
        rtnKit.dir=eventRtnKit::fail;
        return rtnKit;
    }
    if(query.value(0).toInt()==1)//car is leaving
    {
        //get exiting car data
        query.prepare("SELECT * FROM carPresent WHERE plateNo=:plateNo");
        query.bindValue(":plateNo",plateNo);
        query.finish();query.exec();query.next();
        if(query.isValid()==false){qDebug()<<"SQLErr: vehiOutBound browse carPresent failed.";}
        int inTime = query.value("carInTime").toInt();
        int outTime = QDateTime::currentSecsSinceEpoch();
        rtnKit.payPrice = ceil((float)(outTime-inTime)/unitInSec)*pricePerUnit;//calculate price
        query.bindValue(":curTime",QDateTime::currentSecsSinceEpoch());
        //add a payment
        int paymentID=10000+ QRandomGenerator::global()->bounded(40000);//random paymentID 10000~50000
        query.prepare("INSERT INTO payment VALUES (:paymentID, :payPrice, False)");
        query.bindValue(":paymentID",paymentID);
        query.bindValue(":payPrice",rtnKit.payPrice);
        query.exec();
        if(query.isValid()==false){qDebug()<<"SQLErr: vehiOutBound append payment failed.";}
        //add a carLog
        query.prepare("INSERT INTO carLog VALUES (:paymentID, :plateNo, :carinTime, :carOutTime)");
        query.bindValue(":paymentID",paymentID);
        query.bindValue(":plateNo",plateNo);
        query.bindValue(":carinTime",inTime);
        query.bindValue(":carOutTime",outTime);
        query.exec();
        if(query.isValid()==false){qDebug()<<"SQLErr: vehiOutBound append pay to carLog failed.";}
        //delete from carPresent
        query.prepare("DELETE FROM carPresent WHERE plateNo=:plateNo");
        query.bindValue(":plateNo",plateNo);
        query.exec();
        if(query.isValid()==false){qDebug()<<"SQLErr: vehiOutBound romove from carPresent failed.";}
        rtnKit.carInT = QDateTime::fromSecsSinceEpoch(inTime);//unix time to qtime
        rtnKit.carOutT = QDateTime::fromSecsSinceEpoch(outTime);
        rtnKit.dir = eventRtnKit::carOut;
    }else
    {
        qDebug()<<"SQLErr: vehiOutBound get 2+ rtn, WTF?";
        rtnKit.dir = eventRtnKit::fail;
    }
    return rtnKit;
};

QSqlQueryModel* databaseManager::execSQLSelect(QString sqlCmd)
{
    QSqlQuery query(db);
    query.prepare(sqlCmd);//load to query
    query.bindValue(":curTime",QDateTime::currentSecsSinceEpoch());//provide curtime if needed
    query.exec();
    (*openSQLResult).setQuery(query);//auto execute sql command, now result store inside.
    if(openSQLResult->lastError().isValid()==false)
        qDebug()<<"SQLErr: execSQLSelect fail executing outsider command:  "<<openSQLResult->lastError().databaseText();
    return openSQLResult;
}



