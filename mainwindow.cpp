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

    //===============
    QImage imgRaw ("test.png");
    HLPR_ImageData *imageData = new HLPR_ImageData;
    QImage img = imgRaw.convertToFormat(QImage::Format_RGB888);
    imageData->data=img.bits();
    imageData->width=img.width();
    imageData->height=img.height();
    imageData->format=STREAM_RGB;
    imageData->rotation=CAMERA_ROTATION_0;

    HLPR_ContextConfiguration config = {0};

    // =======================================================
    // 核心配置
    // =======================================================
    config.models_path = (char*)"r2_mobile"; // 【設定模型路徑】
    config.threads = 1;                    // 【單執行緒】
    config.use_half = true;                // 【開啟半精度】

    // =======================================================
    // 精度與敏感度配置
    // =======================================================
    config.max_num = 5;                        // 最多檢測 5 個車牌
    config.det_level = DETECT_LEVEL_HIGH;       // 推薦：高開銷/高精度檢測

    // 閾值 (可以根據您的測試情況微調)
    config.box_conf_threshold = 0.5f;          // 檢測框閾值
    config.nms_threshold = 0.3f;               // NMS 閾值
    config.rec_confidence_threshold = 0.7f;    // 識別結果閾值

    buffer = HLPR_CreateDataBuffer(imageData);
    results = {0};
    ctx = HLPR_CreateContext(&config);
    result = HLPR_ContextUpdateStream(ctx, buffer, &results);
    if (result == Ok && results.plate_size > 0) {
        // 處理識別結果
        qDebug()<<"CP8";
        for (unsigned long i = 0; i < results.plate_size; ++i) {
            qDebug() << "Plate No.: " << results.plates[i].code
                      << ", Type of Plate: " << results.plates[i].type
                      << ", Confident: " << results.plates[i].text_confidence;
        }
        // 注意: results.plates 是一個動態分配的數組，HyperLPR SDK 應該會在下次調用
        // 或釋放 Context 時自動管理這塊內存。
    }
    if (result != Ok) {
        qDebug() << "ERROR: HLPR_ContextUpdateStream failed. Result code:" << result;
        // 根據 result 的值（例如 MNN_ERROR, PARAMS_ERROR 等）來診斷問題。
    } else if (results.plate_size == 0) {
        qDebug() << "INFO: Stream update successful, but no plates detected.";
    } else

    // 5. 釋放 Data Buffer
    HLPR_ReleaseDataBuffer(buffer);
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

