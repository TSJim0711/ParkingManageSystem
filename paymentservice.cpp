#include "paymentservice.h"
#define dfAppDd "9021000157673689"

paymentService::paymentService(databaseManager *dbArg)
{
    manager=new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished, this, &paymentService::postRecieveHandler);
    db=dbArg;
    appID=dfAppDd;
    netGate="https://openapi-sandbox.dl.alipaydev.com/gateway.do";
}

void paymentService::raisePay(QString tradeID, QString usrToken, QString payTitle, QString payPrice)
{
    emit paymentUpdate("正在处理支付\n请稍后",10);//send text to main screen
    //compile with: perl Configure no-shared no-legacy no-ssl no-tls no-dso no-ui-console no-err no-comp no-engine no-module no-dtls no-aria no-bf no-camellia no-cast no-des no-dh no-dsa no-ec no-ecdh no-ecdsa no-gost no-idea no-md4 no-mdc2 no-rc2 no-rc4 no-rc5 no-rmd160 no-seed no-srp no-sm2 no-sm3 no-sm4 no-whirlpool --prefix="C:\OpenSSL-Static" --cflags="/FS"
    QMap<QString, QString> allParams;
    // 公共参数（URL 中携带，参与签名）
    allParams["app_id"] = "9021000157673689";       // 你的应用 ID
    allParams["method"] = "alipay.trade.pay";       // 接口名称
    allParams["charset"] = "UTF-8";                 // 编码格式
    allParams["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"); // 当前时间戳
    allParams["version"] = "1.0";                   // 接口版本
    allParams["sign_type"] = "RSA2";                // 签名类型

    // 构造 biz_content（JSON 格式）
    QJsonObject bizContent;
    QString randomString;
    bizContent["out_trade_no"] = tradeID; // 订单号（唯一）
    bizContent["total_amount"] = payPrice;             // 订单金额（两位小数）
    bizContent["subject"] = payTitle;            // 商品标题
    bizContent["auth_code"] = usrToken;//user qr code content
    bizContent["scene"] = "bar_code";//pay method
    // 生成原始 JSON 串（中文不转义，Compact 模式无空格）
    QJsonDocument jsonDoc(bizContent);

    QString bizContentRaw = jsonDoc.toJson(QJsonDocument::Compact);
    allParams["biz_content"] = bizContentRaw;

    //sign ready string (url encode, alipay verify sign with url encode)
    QStringList signPart;
    for (auto it = allParams.begin(); it != allParams.end(); ++it) {
        // 直接拼接原始值（中文保留，不做 URL 编码）
        signPart.append(QString("%1=%2").arg(it.key(), it.value()));
    }
    QString signStr = signPart.join("&");

    //rsa2 sign
    QByteArray sign;//auto assign memory size to max: RSA_size(rsa)
    {
        using namespace std;
        //load RSA private key
        FILE* fp = fopen("rsa.pem", "r");//load private key from file
        if (fp == nullptr)
            qDebug()<<"RSA kye file can not open.";
        RSA* rsa = PEM_read_RSAPrivateKey(fp, nullptr, nullptr, nullptr);
        fclose(fp);
        if (rsa == nullptr)
            qDebug()<<"RSA Key Not loaded. Check RSA struct?";

        //ssh256 hash
        QByteArray signStrUtf8= signStr.toUtf8();
        unsigned char hash[SHA256_DIGEST_LENGTH] = {0};//32 by defualt
        SHA256_CTX sha256_ctx;
        SHA256_Init(&sha256_ctx);
        SHA256_Update(&sha256_ctx, signStrUtf8.constData(), signStrUtf8.length());
        SHA256_Final(hash, &sha256_ctx);
        //rsa sign
        int rsaSize = RSA_size(rsa);
        unsigned char* signBuf = (unsigned char*)malloc(rsaSize);
        unsigned int sig_len = 0;
        int status = RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH,
                              signBuf, &sig_len, rsa);
        RSA_free(rsa);

        if (status != 1) {
            qDebug() << "sign fail";
        }

        sign = QByteArray((const char*)signBuf, sig_len);
        free(signBuf);
    }

    //Sign to base64
    QString signBase64 = sign.toBase64(QByteArray::Base64Encoding|QByteArray::OmitTrailingEquals |QByteArray::Base64UrlEncoding);

    //Request Body build
    QUrlQuery postQuery;
    for (auto it = allParams.begin(); it != allParams.end(); ++it) {
        // 对参数值做 URL 编码（中文转为 %E8%8B%B9%E6%9E%9C 这类格式）
        // 第二个参数：保留 =& 等合法字符，不编码；第三个参数：需要编码的特殊字符（含中文）
        QString encodedValue = QUrl::toPercentEncoding(it.value(), "=&", " !@#$%^&*()_+[]{}|;:,.?~`\u4e00-\u9fa5");
        postQuery.addQueryItem(it.key(), encodedValue);
    }
    // 签名也需要 URL 编码（Base64 中的 +/= 转义为 %2B/%2F/%3D）
    QString encodedSign = QUrl::toPercentEncoding(signBase64);
    postQuery.addQueryItem("sign", encodedSign);
    QByteArray postBody = postQuery.query(QUrl::FullyEncoded).toUtf8();

    //post primary url
    QUrl reqUrl(netGate);
    QNetworkRequest req(reqUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");//state request struct
    req.setRawHeader("Accept", "application/json; charset=UTF-8");//ask for json return

    //post
    manager->post(req, postBody);
};

void paymentService::postRecieveHandler(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    // store return to json
    QByteArray responseData = reply->readAll();
    qDebug()<<"Respond:"<<responseData;

    // decode json
    QString responseStr = QString::fromUtf8(responseData);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();
        QJsonObject coreObj = jsonObj["alipay_trade_pay_response"].toObject();
        QString code = coreObj["code"].toString();
        if (code == "10000")//success
        {
            db->clientPaid(coreObj["out_trade_no"].toString());//set paid in database
            emit paymentUpdate("已缴费\n期待您再次来临~",5);//send text to main screen
        }else
        {
            qDebug() << "Err: Receive respond from alipay: " << coreObj["msg"].toString()<< "，ERRCode：" << code;
            emit paymentUpdate("支付错误！\n"+coreObj["sub_msg"].toString(),5);
        }
    }
    reply->deleteLater();//release reply when ended
}

paymentService::~paymentService()
{
    delete manager;
}
