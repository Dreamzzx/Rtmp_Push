#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QAudioDeviceInfo>
#include <QActionGroup>
#include <QDesktopWidget>
#include <QCamera>
#include <QCameraInfo>

#include "pushwork.h"
#include "rtmpbase.h"
#include "globalhelp.h"
#include "logserve.h"

Q_DECLARE_METATYPE(QCameraInfo)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void init();

    // 开始推流
    int start(std::string url);
    int startLive(std::string url);


    void GetAudioDefaultMicDevice();

    void GetVideoDefaultDevice();

    int OutPutVideo(const Frame* frame);
private slots:

    void on_startBtn_clicked();

private:
    
    LogServe* log_;
    PushWork *push_work_;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
