#include "settingdialog.h"
#include "ui_settingdialog.h"

settingDialog::settingDialog(paymentService *payArg, databaseManager *dbArg, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::settingDialog)
{
    ui->setupUi(this);
    payServ=payArg;
    db=dbArg;
    //load data to line edit as defualt
    ui->leAppId->setText(payServ->appID);
    ui->leNetGate->setText(payServ->netGate);
    ui->leFeePerUnit->setText(QString::number(db->pricePerUnit));
    ui->leSecPerUnit->setText(QString::number(db->unitInSec));
    //limit lineEdit format
    decimalValidator = new QDoubleValidator(0.00, 1000000.00, 2, this);
    intValidator = new QDoubleValidator(0, 1000000, 0, this);
    ui->leFeePerUnit->setValidator(decimalValidator);
    ui->leSecPerUnit->setValidator(intValidator);
    //button click
    connect(ui->buttonBox, &QDialogButtonBox::clicked,this, &settingDialog::handleButtonBoxClicked);
}

void settingDialog::handleButtonBoxClicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);//get which button user clicked
    if(standardButton==QDialogButtonBox::Ok || standardButton==QDialogButtonBox::Apply)
    {
        payServ->appID=ui->leAppId->text();
        payServ->netGate=ui->leNetGate->text();
        db->pricePerUnit=ui->leFeePerUnit->text().toFloat();
        db->unitInSec=ui->leSecPerUnit->text().toInt();
        emit payServ->paymentUpdate("正在更新数据...",0);//borrow a handy function from paymentservice
    }
    if(standardButton==QDialogButtonBox::Ok||standardButton==QDialogButtonBox::Cancel)
    {
        this->close();
        delete this;
    }
}

settingDialog::~settingDialog()
{
    delete ui;
}
