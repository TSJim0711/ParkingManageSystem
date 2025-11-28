#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H
#include <QAbstractButton>
#include <QDoubleValidator>
#include "databasemanager.h"
#include "paymentservice.h"

#include <QDialog>

namespace Ui {
class settingDialog;
}

class settingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit settingDialog(paymentService *payArg, databaseManager *dbArg, QWidget *parent = nullptr);
    ~settingDialog();
private slots:
    void handleButtonBoxClicked(QAbstractButton *button);
private:
    Ui::settingDialog *ui;
    databaseManager *db;
    paymentService *payServ;
    QDoubleValidator *intValidator;
    QDoubleValidator *decimalValidator;
};

#endif // SETTINGDIALOG_H
