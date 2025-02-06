#ifndef H264ENCODER_H
#define H264ENCODER_H

#include "logserve.h"
#include "rtmpbase.h"

class H264Encoder
{
public:
    H264Encoder();

    bool Init(Properties properties,AVRational video_tb);
private:
    int count;
    int framecnt;

    // 初始化参数
    std::string codec_name_;  //
    int width_;     // 宽
    int height_;    // 高
    int fps_; // 帧率
    int b_frames_;   // b帧数量
    int bitrate_;
    int gop_;
    bool annexb_;       // 默认不带star code
    int threads_;
    std::string profile_;
    std::string level_id_;

    std::string sps_;
    std::string pps_;

    //encoder message
    AVCodec* codec_ = NULL;
    AVDictionary *param = NULL;
    AVCodecContext* ctx_ = NULL;

    int64_t pts_ = 0;

};

#endif // H264ENCODER_H
