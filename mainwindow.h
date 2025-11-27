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
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "hyper_lpr_sdk.h"
#include "databasemanager.h"
#include "bussinessdataviewer.h"
#include "paymentservice.h"


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
    void showText(QString txt);

private slots:
    void on_btCameraToggle_clicked();
    void processVideoFrame(const QVideoFrame &frame);
    void on_btDataView_clicked();

private:
    Ui::MainWindow *ui;
    QList<QCameraDevice> cameralist;
    QCamera *camera;
    QMediaCaptureSession *captureSession;
    QVideoSink *videoFrameFlow;
    databaseManager *dbManager;
    bussinessDataViewer *dataViewer;
    paymentService *payServc;

    //HyperLPR
    HLPR_ImageData *imageData;
    P_HLPR_DataBuffer buffer;
    HLPR_PlateResultList results;
    P_HLPR_Context ctx;
    HREESULT result;

    //HyperLPR result vote
    int frameIdxPlateRec;
    QString curPlate;//curPlate is determine by multi live plate in short time, according to highest plateVote
    QPair<QString,float> livePlate;
    QQueue<QPair<QString,float>> plateVote;
    int plateVoteFlag;
    databaseManager::eventRtnKit rtnKit;

    //QR Code Scan
    int frameIdxQRCode;
    bool QRScanFlag;
    cv::QRCodeDetector qrDecoder;
};
#endif // MAINWINDOW_H
