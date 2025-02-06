#ifndef PUSHWORK_H
#define PUSHWORK_H

#include <functional>

#include "aacencoder.h"
#include "h264encoder.h"
#include "rtmpbase.h"
#include "globalhelp.h"
#include "logserve.h"
#include "rtmppush.h"

class PushWork
{
public:
    PushWork();

    /**
     * @brief 推流初始化，只支持H264+AAC格式
     * @param properties
     *      音频test模式：
     *          "audio_test": 缺省为0，为1时数据读取本地文件进行播放
     *          "input_pcm_name": 测试模式时读取的文件路径
     *      麦克风采样属性：
     *          "mic_sample_fmt", 麦克风采样格式  AVSampleFormat对应的值，缺省为AV_SAMPLE_FMT_S16
     *          "mic_sample_rate", 麦克风采样率，缺省为480000
     *          "mic_channels", 麦克风采样通道，缺省为2通道
     *      音频编码属性：
     *          "audio_sample_rate", 采样率, 缺省和麦克风采样率一致
     *          "audio_bitrate"   , 编码比特率, 缺省为128*1024bps
     *          "audio_channels"  , 编码通道，缺省为2通道
     *      视频test模式：
     *          "test_test": 缺省为0，为1时数据读取本地文件进行播放
     *          "input_yuv_name": 测试模式时读取的文件路径
     *      桌面录制属性：
     *          "desktop_x", x起始位置，缺省为0
     *          "desktop_y", y起始位置，缺省为0
     *          "desktop_width", 宽度，缺省为屏幕宽带
     *          "desktop_height", 高度，缺省为屏幕高度
     *          "desktop_pixel_format", 像素格式，AVPixelFormat对应的值，缺省为AV_PIX_FMT_YUV420P
     *          "desktop_fps", 帧数，缺省为25
     *      视频编码属性：
     *          "video_width", 视频宽度，缺省值和desktop_width一致
     *          "video_height", 视频高度，缺省值和desktop_height一致
     *          "video_fps",    帧率, 缺省值和desktop_fps一致
     *          "video_gop",  i帧间隔，默认和帧率一致
     *          "video_bitrate", 视频bitrate, 可以自己设置，缺省值则根据不同分辨率自动进行调整
     *          "video_b_frames", 连续B帧最大数量，缺省为0，直播一般都不回用设置b帧
     *      rtmp推流：
     *          "rtmp_url", 推流地址
     *          "rtmp_debug", 默认为0, 为1则为debug, 实时显示推流的画面，方便和拉流进行延迟对比
     *
     * @return
     */
    bool Init(const Properties &properties);

    void AddVideoPreviewCallBack(std::function<int(const Frame*)> callback);

private:

    std::string audio_device_name_;
    std::string video_device_name_;

    // 音频参数
    int audio_sample_rate_;
    int audio_bitrate_;
    int audio_channels_;
    int audio_sample_fmt_;
    AVChannelLayout* audio_ch_layout_;
   // int audio_ch_layout_;

    // 视频参数
    int video_width_;
    int video_height_;
    int video_fps_;
    int video_gop_;
    int video_bitrate_;
    int video_b_frames_;

    // 桌面参数
    int desktop_x_, desktop_y_, desktop_width_, desktop_height_;
    int desktop_format_;
    int desktop_fps_;


    RtmpPush*rtmp_push_;
    std::string rtmp_url_; // rtmp推流地址

    AVPacket*audio_pkt_;
    AACEncoder*audio_encoder_;

    AVPacket*video_pkt_;
    H264Encoder*video_encoder_;
    // 视频预览回调函数  &MainWindow::OutputVideo
    std::function<int(const Frame*)> video_preview_callback_ = nullptr;
};

#endif // PUSHWORK_H
