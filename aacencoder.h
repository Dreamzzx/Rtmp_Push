#ifndef AACENCODER_H
#define AACENCODER_H
#include "rtmpbase.h"

class AACEncoder
{
public:
    AACEncoder();

    bool Init(Properties properties,AVRational aduio_tb);

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
