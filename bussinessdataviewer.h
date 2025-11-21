#ifndef BUSSINESSDATAVIEWER_H
#define BUSSINESSDATAVIEWER_H

#include <QDialog>
#include <QListWidgetItem>
#include <QList>
#include "databasemanager.h"

namespace Ui {
class bussinessDataViewer;
}

class bussinessDataViewer : public QDialog
{
    Q_OBJECT

public:
    explicit bussinessDataViewer(QWidget *parent = nullptr, databaseManager *dbArg = nullptr);
    ~bussinessDataViewer();

private slots:
    void listViewOnClick(const QModelIndex &index);

private:
    Ui::bussinessDataViewer *ui;
    databaseManager *dbClass;
    QListWidgetItem *listItem;
    void addListViewOption(QString dispTxt, QString SQLQuery);
    QList <QString> sqlCmdList;
};

#endif // BUSSINESSDATAVIEWER_H
