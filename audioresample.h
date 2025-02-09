#ifndef AUDIORESAMPLE_H
#define AUDIORESAMPLE_H

#include"globalhelp.h"

class AudioResample
{
public:
    typedef struct ResampleParams
    {
        // in
        AVChannelLayout srcChannelLayout;
        int srcSampleFmt;
        int srcSampleRate;
        // out
        AVChannelLayout dstChannelLayout;
        int dstSampleFmt;
        int dstSampleRate;
    }ResampleParams;

    AudioResample();

    // 进行重采样
    int send_frame(uint8_t **in_data,int in_nb_samples,uint64_t pts);
    // 将重采样的数据进行返回
    int recive_frame(int nb_samples);

    bool Init(const ResampleParams params);

private:

    int is_fifo_only_= 0; // 不需要进行重采样

    SwrContext* swr_ctx = nullptr;
    ResampleParams params_;

    AVAudioFifo *audio_fifo_ = nullptr; // 重采样的缓冲区

    uint8_t **resampled_data=nullptr;       // 用来缓存重采样后的数据
    int resampled_data_size = 0; // 重采样后的采样数
    int dst_nb_sample = 0;
};

#endif // AUDIORESAMPLE_H
