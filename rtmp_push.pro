QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += multimedia

CONFIG += c++17

QMAKE_TARGET = x86_64

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/lib/ffmpeg6.1.2/include

INCLUDEPATH += $$PWD/librtmp/include

LIBS += $$PWD/librtmp/lib/librtmp.lib

LIBS += $$PWD/lib/ffmpeg6.1.2/lib/avformat.lib   \
        $$PWD/lib/ffmpeg6.1.2/lib/avcodec.lib    \
        $$PWD/lib/ffmpeg6.1.2/lib/avdevice.lib   \
        $$PWD/lib/ffmpeg6.1.2/lib/avfilter.lib   \
        $$PWD/lib/ffmpeg6.1.2/lib/avutil.lib     \
        $$PWD/lib/ffmpeg6.1.2/lib/postproc.lib   \
        $$PWD/lib/ffmpeg6.1.2/lib/swresample.lib \
        $$PWD/lib/ffmpeg6.1.2/lib/swscale.lib    \




SOURCES += \
    aacencoder.cpp \
    displaywidget.cpp \
    globalhelp.cpp \
    h264encoder.cpp \
    logserve.cpp \
    main.cpp \
    mainwindow.cpp \
    pushwork.cpp \
    rtmpbase.cpp \
    rtmppush.cpp

HEADERS += \
    aacencoder.h \
    displaywidget.h \
    globalhelp.h \
    h264encoder.h \
    logserve.h \
    mainwindow.h \
    pushwork.h \
    rtmpbase.h \
    rtmppush.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


