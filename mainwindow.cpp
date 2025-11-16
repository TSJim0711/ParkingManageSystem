#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cameralist = QMediaDevices::videoInputs();
    for(int i=0; i<cameralist.size();i++)
        qDebug()<<"发现相机: "<<cameralist[i].description()<<"; "<<cameralist[i].id();
    captureSession = new QMediaCaptureSession(this);
    camera=new QCamera(cameralist[0]);
    QVideoWidget *wgt = new QVideoWidget(ui->widget) ;
    wgt->resize(ui->widget->width(),ui->widget->height());
    wgt->show();
    captureSession->setCamera(camera);           // connect camera and widget
    captureSession->setVideoOutput(wgt);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btCameraToggle_clicked()
{

    if(camera->isActive()==false)
        camera->start();
    else
        camera->stop();
}

