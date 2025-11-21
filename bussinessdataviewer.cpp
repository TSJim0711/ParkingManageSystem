#include "bussinessdataviewer.h"
#include "ui_bussinessdataviewer.h"

bussinessDataViewer::bussinessDataViewer(QWidget *parent, databaseManager *dbArg)
    : QDialog(parent)
    , ui(new Ui::bussinessDataViewer)
{
    ui->setupUi(this);
    dbClass=dbArg;
    addListViewOption("在停车辆","SELECT plateNo, printf('T+ %s',time(:curTime - carInTime, 'unixepoch')) AS '停放时间' FROM carPresent");
    addListViewOption("收费历史列表","SELECT carLog.paymentID, plateNo, amount AS '应缴', CASE WHEN paidFlag  THEN '已缴费' ELSE '未缴费' END AS 缴费情况 FROM carLog INNER JOIN payment ON carLog.paymentID = payment.paymentID");
    addListViewOption("常客列表","SELECT plateNo, COUNT(*) AS '频次' FROM carLog GROUP BY plateNo ORDER BY '频次' DESC");
    connect(ui->lwDataShowSelector,&QListView::clicked,this,&bussinessDataViewer::listViewOnClick);
}

bussinessDataViewer::~bussinessDataViewer()
{
    delete ui;
}

void bussinessDataViewer::addListViewOption(QString dispTxt, QString sqlQuery)
{
    ui->lwDataShowSelector->addItem(dispTxt);
    sqlCmdList.append(sqlQuery);
}

void bussinessDataViewer::listViewOnClick(const QModelIndex &index)
{
    qDebug()<<sqlCmdList.at(index.row());
    QSqlQueryModel *sqlResult = dbClass->execSQLSelect(sqlCmdList.at(index.row()));
    ui->tableView->setModel(sqlResult);
}
