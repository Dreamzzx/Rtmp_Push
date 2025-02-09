#include "rtmpbase.h"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

#include "rtmp_sys.h"
#include <QDebug>

#ifdef _MSC_VER
#define strncasecmp strnicmp
#endif

RtmpBase::RtmpBase()
{
    initRtmp();
}

bool RtmpBase::Connect(std::string url)
{
    url_ = url;
    return Connect();
}

bool RtmpBase::Connect()
{
    RTMP_Free(rtmp_);
    rtmp_ = RTMP_Alloc();
    RTMP_Init(rtmp_);
    qDebug() << "base begin connect";
    //set connection timeout,default 30s
    rtmp_->Link.timeout = 10;
    if (RTMP_SetupURL(rtmp_, (char*)url_.data()) < 0) {
        qDebug()  << "RTMP_SetupURL failed!";
        return FALSE;
    }
    //用于直播流
    rtmp_->Link.lFlags |= RTMP_LF_LIVE;             //  then we can't seek/resume
    RTMP_SetBufferMS(rtmp_, 3600 * 1000);           //1hour

    RTMP_EnableWrite(rtmp_);    /*默认是play, 设置可写publish,即发布流,这个函数必须在连接前使用,否则无效*/

    if (!RTMP_Connect(rtmp_, NULL)) {
        qDebug() <<"RTMP_Connect failed!";
        return FALSE;
    }
    if (!RTMP_ConnectStream(rtmp_, 0)) {
      //  LOG(INFO) << "RTMP_ConnectStream failed";
        return FALSE;
    }
    //判断是否打开音视频,默认打开
    return true;
}

void RtmpBase::initRtmp()
{
    bool ret_code = true;
#ifdef WIN32
    //windows初始化网络库
    WORD version;
    WSAData wsaData;
    version = MAKEWORD(1,1);
    ret_code = (WSAStartup(version, &wsaData) == 0) ? true : false;
#endif

    rtmp_ = RTMP_Alloc();
    RTMP_Init(rtmp_);
    if(ret_code == false)
    {
       //LogError("WSAStartup is failed!");
    }
}

bool RtmpBase::IsConnect()
{
    return RTMP_IsConnected(rtmp_);
}
