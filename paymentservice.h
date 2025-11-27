#ifndef PAYMENTSERVICE_H
#define PAYMENTSERVICE_H

#include <QMainWindow>
#include <QObject>
#include <QUrlQuery>
#include <QHttpPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include "databasemanager.h"

class paymentService : public QObject
{
    Q_OBJECT
public:
    paymentService(databaseManager *dbArg);
    ~paymentService();
    void raisePay(QString tradeID, QString usrToken, QString payTitle, QString payPrice);
signals:
    void paymentUpdate(QString detail);
private slots:
    void postRecieveHandler(QNetworkReply* reply);
private:
    QNetworkAccessManager *manager;
    databaseManager *db;
};

#endif // PAYMENTSERVICE_H
