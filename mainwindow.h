#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QCamera>
#include <QVideoWidget>
#include <QMediaCaptureSession>

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
};
#endif // MAINWINDOW_H
