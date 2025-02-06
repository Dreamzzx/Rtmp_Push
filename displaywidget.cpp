#include "displaywidget.h"
#include <QDebug>
DisplayWidget::DisplayWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setStyleSheet("background-color:black;");
    // 父控件的样式表
    this->setStyleSheet("QWidget#previewWidget { background-color: black; }");
}

int DisplayWidget::Draw(const Frame *frame)
{
    return 0;
}
