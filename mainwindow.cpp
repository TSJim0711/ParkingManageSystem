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
    videoFrameFlow = new QVideoSink(this);
    captureSession->setCamera(camera);           // connect camera and widget
    captureSession->setVideoSink(videoFrameFlow);

    HLPR_ContextConfiguration config = {0};
    // =======================================================
    // 核心配置
    // =======================================================
    config.models_path = (char*)"r2_mobile"; // 【設定模型路徑】
    config.threads = 12;                    // 【單執行緒】
    config.use_half = true;                // 【開啟半精度】

    // =======================================================
    // 精度與敏感度配置
    // =======================================================
    config.max_num = 1;                        // 最多檢測 5 個車牌
    config.det_level = DETECT_LEVEL_LOW;       // 推薦：高開銷/高精度檢測

    // 閾值 (可以根據您的測試情況微調)
    config.box_conf_threshold = 0.5f;          // 檢測框閾值
    config.nms_threshold = 0.3f;               // NMS 閾值
    config.rec_confidence_threshold = 0.7f;    // 識別結果閾值

    //Buffer size create
    imageData=new HLPR_ImageData();
    imageData->width=0;
    imageData->height=0;
    imageData = new HLPR_ImageData;
    imageData->format=STREAM_RGB;
    imageData->rotation=CAMERA_ROTATION_0;

    buffer = HLPR_CreateDataBuffer(imageData);
    ctx = HLPR_CreateContext(&config);
    frameIdx=0;
    plateVoteFlag=0;
    connect(videoFrameFlow, &QVideoSink::videoFrameChanged, this, &MainWindow::processVideoFrame);
}

MainWindow::~MainWindow()
{
    HLPR_ReleaseDataBuffer(buffer); //release buffer addr
    delete ui;
}

void MainWindow::on_btCameraToggle_clicked()
{

    if(camera->isActive()==false)
        camera->start();
    else
        camera->stop();
}

void  MainWindow::processVideoFrame(const QVideoFrame &frame)
{
    QImage img=frame.toImage().convertToFormat(QImage::Format_RGB888);//frame to image
    QPixmap pixmap = QPixmap::fromImage(img);
    QPixmap scaledPixmap = pixmap.scaled(ui->lbCamLive->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->lbCamLive->setPixmap(scaledPixmap);
    frameIdx++;//30 frame 1 runs, 8 frame when voting
    if(frameIdx<(plateVoteFlag>0?8:30)||!frame.isValid())
        return;

    frameIdx=0;

    if(img.width()!=imageData->width || img.height()!=imageData->height)//determine if camera resulotion fit size of buffer
    {
        imageData->data=img.bits();
        HLPR_ReleaseDataBuffer(buffer);//release out-dated buffer
        imageData->width=img.width();//update to current res
        imageData->height=img.height();
        buffer = HLPR_CreateDataBuffer(imageData);//assign correct buffer size
    }else
    {
        HLPR_DataBufferSetData(buffer,img.constBits(),img.width(),img.height());//update buf, point to newest img.bits()
    }

    results = {0};//clear last result
    result = HLPR_ContextUpdateStream(ctx, buffer, &results);

    if (result == Ok && results.plate_size > 0) {
        // 處理識別結果
        for (unsigned long i = 0; i < results.plate_size; ++i) {
            qDebug() << "Plate No.: " << results.plates[i].code
                     << ", Type of Plate: " << results.plates[i].type
                     << ", Confident: " << results.plates[i].text_confidence;

            livePlate.first=results.plates[i].code;
            livePlate.second=results.plates[i].text_confidence;
        }
    }

    if (result != Ok) {
        qDebug() << "ERROR: HLPR_ContextUpdateStream failed. Result code:" << result;
        // 根據 result 的值（例如 MNN_ERROR, PARAMS_ERROR 等）來診斷問題。
    } else if (results.plate_size == 0) {
        qDebug() << "Stream update success, but no plates detected.";
        livePlate.first="";
        livePlate.second=0.8;
    }

    if(livePlate.first!=curPlate && plateVoteFlag<=0)//detect new plate, start vote
    {
        plateVoteFlag=6;//5 vote run, 8 frame per run
        plateVote.clear();
        if(livePlate.first!="")
            ui->lbVehiPlateNumd->setText("停定！检测中！");
        qDebug()<<"Vote Start!!";
    }
    if(plateVoteFlag>0)//plate vote
    {
        plateVoteFlag--;
        int idx;
        for(idx=0;idx<plateVote.size();idx++)
            if(plateVote.at(idx).first==livePlate.first)
            {
                plateVote[idx].second+=livePlate.second;//add conf to sum of plate
                qDebug()<<"Plate No.: "<<plateVote[idx].first<<", SumConf: "<<plateVote[idx].second;
                if(plateVote[idx].second>2.5)//reach major (>2.5 of 5), win now
                    plateVoteFlag=1;
                break;
            }
        if(idx>=plateVote.size())//livePlate not in queue, append it
        {
            plateVote.append(livePlate);
            qDebug()<<"Plate No.: "<<plateVote.back().first<<", NewConf: "<<plateVote.back().second;
        }
    }
    if(plateVoteFlag==1)//plate vote end
    {
        QPair<QString,float> maxConfPlate={"",0};
        for(int idx=0;idx<plateVote.size();idx++)
            if(plateVote[idx].second>maxConfPlate.second)//take most conf plate
                maxConfPlate=plateVote[idx];
        qDebug()<<"Vote End!! Wins: "<<maxConfPlate.first<<" SumConf: "<<maxConfPlate.second;
        curPlate=maxConfPlate.first;
        if(curPlate=="")//display
            ui->lbVehiPlateNumd->setText("欢迎莅临\nXYZ停车场");
        else
            ui->lbVehiPlateNumd->setText(curPlate);
        plateVoteFlag=0;
    }
}
