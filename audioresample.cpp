#include "audioresample.h"
#include "logserve.h"
AudioResample::AudioResample()
{
    LogInfo("AudioResample Init");
    //Init();
}

int AudioResample::send_frame(uint8_t **in_data, int in_nb_samples, uint64_t pts)
{

    return 0;
}

bool AudioResample::Init(const ResampleParams params)
{
    if( params.srcSampleFmt == params.dstSampleFmt &&
        params.srcSampleRate == params.dstSampleRate &&
        params.srcChannelLayout.u.mask == params.dstChannelLayout.u.mask)
    {
        is_fifo_only_ = 1;
        return 1;
    }

    this->params_ = params;
    audio_fifo_ = av_audio_fifo_alloc((enum AVSampleFormat )params.dstSampleFmt,
                                      params.dstChannelLayout.nb_channels,
                                      1);
    if(audio_fifo_ == nullptr)
    {
        LogError("av_audio_fifo_alloc is failed!");
        return false;
    }

    // 初始化重采样
    this->swr_ctx = swr_alloc();
    if(swr_ctx == nullptr)
    {
        LogError("swr_alloc is failed!");
        return false;
    }
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt",      (enum AVSampleFormat)this->params_.srcSampleFmt, 0);
    av_opt_set_chlayout(swr_ctx,   "in_channel_layout",  &this->params_.srcChannelLayout, 0);
    av_opt_set_int(swr_ctx,        "in_sample_rate",     this->params_.srcSampleRate, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt",     (enum AVSampleFormat)this->params_.dstSampleFmt, 0);
    av_opt_set_chlayout(swr_ctx,   "out_channel_layout", &this->params_.dstChannelLayout, 0);
    av_opt_set_int(swr_ctx,        "out_sample_rate",    this->params_.dstSampleRate, 0);

    if(swr_init(swr_ctx) < 0)
    {
        LogError("swr_init is failed!");
        return false;
    }

    // 分配dst缓冲区
    dst_nb_sample = resampled_data_size = 8096 * 2;
    if(resampled_data)
        av_freep(*resampled_data);
    int linesize = 0;
    //动态分配音频样本数据
    int ret = av_samples_alloc_array_and_samples(&resampled_data        //存储分配的音频数据缓冲区
                                                 ,&linesize             //存储每个音频通道数据的步长(以字节为单位)。
                                                 ,params.dstChannelLayout.nb_channels
                                                 ,dst_nb_sample
                                                 ,(enum AVSampleFormat)params.dstSampleFmt   //音频样本格式,如 AV_SAMPLE_FMT_S16、AV_SAMPLE_FMT_FLTP 等。
                                                 ,1);                   //内存对齐要求
    if(ret < 0)
        LogError("fail accocate audio resampled data buffer");

    return true;
};
