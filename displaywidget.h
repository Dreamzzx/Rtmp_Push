#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>
#include <rtmpbase.h>
#include <QMutex>

class DisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayWidget(QWidget *parent = nullptr);

    int Draw(const Frame*frame);
signals:

private:

    int video_width_ = 0;
    int video_height_ = 0;

    int img_width_ = 0;
    int img_height_ = 0;

    int widget_width_ = 0;
    int widget_height_ = 0;

    QMutex mtx_;

    int play_state_;
};

#endif // DISPLAYWIDGET_H
