#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    // 打开日志
    log_ = LogServe::instance();
    log_->open("log.log");
    GetAudioDefaultMicDevice();
    GetVideoDefaultDevice();
    // connect

}

int MainWindow::start(std::string url)
{
    int ret = -1;
    LogInfo(u8"正在打开Live");
    ret = startLive(url);
    if(ret < 0)
    {
        LogError("打开Live失败!");
        return -1;
    }

    return 0;
}

int MainWindow::startLive(std::string url)
{
    int ret = -1;

//    if(url.empty())
//    {
//        error(u8"url为空!");
//        return -1;
//    }

    Properties properties;

    properties.SetProperty("audio_device_name",ui->Mic_comboBox->currentText().toStdString());
    int audio_ns = -1,audio_agc = 0;
    properties.SetProperty("audio_ns", audio_ns);
    properties.SetProperty("audio_agc", audio_agc);

    // 设置视频采集设备名称为"video=screen-capture-recorder"
    properties.SetProperty("video_device_name", "video=screen-capture-recorder");

    // 设置音频编码属性:采样率、声道数、码率,固定使用AAC编码 48000
    properties.SetProperty("audio_sample_rate", 16000);
    properties.SetProperty("audio_channels", 2);
    properties.SetProperty("audio_bitrate", 128 * 1024);

    // 设置桌面录制属性:分辨率、帧率、码率、像素格式,固定使用H.264编码
    QDesktopWidget *qDesktopWidget = QApplication::desktop();
    QRect qrect1 = qDesktopWidget->screen(0)->rect();
    int desktop_width = qrect1.width();
    int desktop_height = qrect1.height();
    properties.SetProperty("desktop_x", 0);
    properties.SetProperty("desktop_y", 0);
    properties.SetProperty("desktop_width", desktop_width);
    properties.SetProperty("desktop_height", desktop_height);
    properties.SetProperty("desktop_pixel_format", AV_PIX_FMT_YUV420P);
    properties.SetProperty("desktop_fps", 25); // 测试模式时和yuv文件的帧率一致

    // 设置视频编码属性:码率
    properties.SetProperty("video_bitrate", 3 * 1024 * 1024);
    // 设置视频采集设备名称为"video=screen-capture-recorder"
    properties.SetProperty("video_device_name", "video=screen-capture-recorder");

    //设置RTMP推流属性
    properties.SetProperty("rtmp_url", url.c_str());

    // 设置推送对象
    push_work_ = new PushWork();
    push_work_->AddVideoPreviewCallBack(std::bind(&MainWindow::OutPutVideo,this,std::placeholders::_1));

    if(push_work_->Init(properties) == false)
    {
        delete push_work_;
        push_work_ == nullptr;
        LogError("push_work_ init failed!");
        return -1;
    }
    return 0;
}

void MainWindow::GetAudioDefaultMicDevice()
{
    QActionGroup* audioDevicesGroup = new QActionGroup(this);
    QList<QAudioDeviceInfo> inputDeviceInfo;
    inputDeviceInfo = QAudioDeviceInfo::availableDevices(QAudio::Mode::AudioInput);
    for (const QAudioDeviceInfo &audioInfo : inputDeviceInfo)
    {
        QAction* audioDeviceAction = new QAction(audioInfo.deviceName(), audioDevicesGroup);
        audioDeviceAction->setCheckable(true);
        audioDeviceAction->setData(QVariant::fromValue(audioInfo));
        if (audioInfo.deviceName() == QAudioDeviceInfo::defaultInputDevice().deviceName()); //无作用的if条件
        {
            audioDeviceAction->setChecked(true);
        }
        ui->Mic_comboBox->addItem(audioInfo.deviceName(), QVariant::fromValue(audioInfo));
    }

}

void MainWindow::GetVideoDefaultDevice()
{
   // qRegisterMetaType<QCameraInfo>("QCameraInfo");
    QActionGroup* videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);//添加互斥性
    QList<QCameraInfo>cameraInfos = QCameraInfo::availableCameras();
    for(const QCameraInfo &cameraInfo:cameraInfos)
    {
        QAction* videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
        if (cameraInfo == QCameraInfo::defaultCamera()) {
            videoDeviceAction->setChecked(true);
        }
        ui->show_comboBox->addItem(cameraInfo.description(), QVariant::fromValue(cameraInfo));
    }
}

int MainWindow::OutPutVideo(const Frame *frame)
{
    return ui->previewWidget->Draw(frame);
}

void MainWindow::on_startBtn_clicked()
{
    if(true)
    {
        start(ui->rtmp_lineEdit->text().toStdString());
    }
}

