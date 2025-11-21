#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QCamera>
#include <QVideoWidget>
#include <QMediaCaptureSession>
#include <QtMultimedia/QVideoSink>
#include <QQueue>
#include "hyper_lpr_sdk.h"
#include "databasemanager.h"
#include "bussinessdataviewer.h"

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
    void processVideoFrame(const QVideoFrame &frame);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_btDataView_clicked();

private:
    Ui::MainWindow *ui;
    QList<QCameraDevice> cameralist;
    QCamera *camera;
    QMediaCaptureSession *captureSession;
    QVideoSink *videoFrameFlow;
    databaseManager *dbManager;
    bussinessDataViewer *dataViewer;

    //HyperLPR
    HLPR_ImageData *imageData;
    P_HLPR_DataBuffer buffer;
    HLPR_PlateResultList results;
    P_HLPR_Context ctx;
    HREESULT result;

    //HyperLPR result vote
    int frameIdx;
    QString curPlate;//curPlate is determine by multi live plate in short time, according to highest plateVote
    QPair<QString,float> livePlate;
    QQueue<QPair<QString,float>> plateVote;
    int plateVoteFlag;
};
#endif // MAINWINDOW_H
