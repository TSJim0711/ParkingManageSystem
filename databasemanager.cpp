#include "databasemanager.h"
#include <QSqlError>

databaseManager::databaseManager()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("1.db");
    db.open();
    QSqlQuery query(db);
    bool queryValid = query.exec("CREATE TABLE IF NOT EXISTS carList (plateNo TEXT NOT NULL, carInTime INT NOT NULL, carOutTime INT, PRIMARY KEY (plateNo, carInTime))");//unix time
    if(queryValid==false)
        qDebug()<<"SQLErr: Create table fail when needed.";
}
databaseManager::~databaseManager()
{
    if (db.isOpen())
        db.close();
}

databaseManager::eventRtnKit databaseManager::vehiScanned(QString plateNo)
{
    eventRtnKit rtnKit;
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM carList WHERE plateNo=:plateNo AND carOutTime IS NULL");
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
        qDebug()<<"Size"<<query.value(0).toInt();
        rtnKit = vehiOutBound(plateNo);
    }
    return rtnKit;
}

bool databaseManager::vehiInBound(QString plateNo)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO carList (plateNo, carInTime) VALUES(:plateNo, :time)");
    query.bindValue(":plateNo",plateNo);
    query.bindValue(":time",QDateTime::currentSecsSinceEpoch());
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
    query.prepare("SELECT COUNT(*) FROM carList WHERE plateNo=:plateNo AND carOutTime IS NULL");
    query.bindValue(":plateNo",plateNo);
    query.finish();query.exec();query.next();
    if(query.isValid()==false)
    {
        qDebug()<<"SQLErr: vehiOutBound update vehi to DB failed.";
        rtnKit.dir=eventRtnKit::fail;
        return rtnKit;
    }
    if(query.value(0).toInt()==1)
    {
        query.prepare("UPDATE carList SET carOutTime=:time WHERE plateNo=:plateNo AND carOutTime IS NULL RETURNING *");
        query.bindValue(":time",QDateTime::currentSecsSinceEpoch());
        query.bindValue(":plateNo",plateNo);
        query.finish();query.exec();query.next();
        int inTime = query.value("carInTime").toInt();
        int outTime = query.value("carOutTime").toInt();
        qDebug()<<"outTime"<<outTime<<"Intime:"<<inTime<<"outTime-inTime:"<<outTime-inTime;
        rtnKit.payPrice = ceil((float)(outTime-inTime)/unitInSec)*pricePerUnit;
        rtnKit.carInT = QDateTime::fromSecsSinceEpoch(inTime);
        rtnKit.carOutT = QDateTime::fromSecsSinceEpoch(outTime);
        rtnKit.dir = eventRtnKit::carOut;
    }else
    {
        qDebug()<<"SQLErr: vehiOutBound get 2+ rtn, WTF?";
        rtnKit.dir = eventRtnKit::fail;
    }
    return rtnKit;
};



