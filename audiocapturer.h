#ifndef AUDIOCAPTURER_H
#define AUDIOCAPTURER_H

#include "rtmpbase.h"
#include "globalhelp.h"
#include "audioresample.h"
#include <functional>
#include <thread>

class AudioCapturer
{
public:
    AudioCapturer();

    bool Init(const Properties &properties,AVChannelLayout audio_enc_channel_layout);

    bool Start();

    void AddCallback(std::function<void(AVFrame*)> callback)
    {
        callback_get_frame_ = callback;
    }

    bool initResample();

    void loop();

    bool Resample_Process();

private:

    // 编码参数
    int audio_enc_sample_rate_;
    int audio_enc_sample_fmt_;
    AVChannelLayout audio_enc_channle_layout_;
    int audio_enc_frame_size_;

    //采样参数
    int capture_sample_rate_ = 48000;
    int capture_sample_fmt_;
    AVChannelLayout capture_channel_layout_;
    int capture_frame_size_;

    // 音频采集相关
    std::string device_name_;
    AVFormatContext	*ifmt_ctx_ = nullptr;
    int             audio_stream_ = -1;
    AVCodecContext	*codec_ctx_ = nullptr;
    AVFrame	*capture_frame_ = nullptr;


    AVFrame*encode_frame_ = nullptr;
    std::function<void(AVFrame*)> callback_get_frame_ = nullptr;

    std::unique_ptr<std::thread> loop_thread_;

    std::unique_ptr<AudioResample>  capture2encode_resmaple_;
};

#endif // AUDIOCAPTURER_H
