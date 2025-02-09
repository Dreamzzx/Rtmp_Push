#ifndef AACENCODER_H
#define AACENCODER_H
#include "rtmpbase.h"

class AACEncoder
{
public:
    AACEncoder();

    bool Init(Properties properties,AVRational aduio_tb);

    int Input(const AVFrame* frame);

    int Output(AVPacket*pkt);

    int get_sample_rate(){
        return ctx_->sample_rate;
    }

    int get_sample_fmt()
    {
        return ctx_->sample_fmt;
    }

    AVChannelLayout get_channel_layout()
    {
        return ctx_->ch_layout;
    }

    int get_frame_size()
    {
        return ctx_->frame_size;
    }

    AVRational get_time_base()
    {
        return ctx_->time_base;
    }

    int get_profile()
    {
        return ctx_->profile;
    }
private:

    AVCodec 	*codec_ = nullptr;
    AVCodecContext	*ctx_ = nullptr;
    AVFrame         *frame_ = nullptr;

    int sample_rate_; // 默认 48000
    int channels_;    //
    int bitrate_;    //    默认out_samplerate*3
    //int channel_layout_;  //  默认AV_CH_LAYOUT_STEREO
    AVChannelLayout *ch_layout_;
    int	frame_byte_size_;      // 一帧的输入byte size
};

#endif // AACENCODER_H
