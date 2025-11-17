#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QCamera>
#include <QVideoWidget>
#include <QMediaCaptureSession>
#include "hyper_lpr_sdk.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btCameraToggle_clicked();

private:
    Ui::MainWindow *ui;
    QList<QCameraDevice> cameralist;
    QCamera *camera;
    QMediaCaptureSession *captureSession;

    P_HLPR_DataBuffer buffer;
    HLPR_PlateResultList results;
    P_HLPR_Context ctx;
    HREESULT result;

};
#endif // MAINWINDOW_H
