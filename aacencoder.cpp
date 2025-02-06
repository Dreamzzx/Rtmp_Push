#include "aacencoder.h"
#include "logserve.h"

AACEncoder::AACEncoder()
{

}

bool AACEncoder::Init(Properties properties, AVRational audio_tb)
{
    // 获取参数
    sample_rate_ = properties.GetProperty("sample_rate", 48000);
    bitrate_ = properties.GetProperty("bitrate", 128 * 1024);
    channels_ = properties.GetProperty("channels", 2);
//    channel_layout_ = properties.GetProperty("channel_layout",
//                                             (int)av_get_default_channel_layout(channels_));
    ch_layout_ = new AVChannelLayout();
    av_channel_layout_default(ch_layout_,channels_);

    int ret;
    // 读取默认的aac encoder
    codec_ = (AVCodec *)avcodec_find_encoder(AV_CODEC_ID_AAC);
    if(!codec_) {
        LogError( "AAC: No encoder found");
        return false;
    }
    ctx_ = avcodec_alloc_context3(codec_);
    if (ctx_ == NULL) {
        LogError("AAC: could not allocate context.");
        return false;
    }
    //Set params
    ctx_->ch_layout.nb_channels		= channels_;
    ctx_->ch_layout	= *ch_layout_;
    ctx_->sample_fmt		= AV_SAMPLE_FMT_FLTP;
    ctx_->sample_rate	= sample_rate_;
    ctx_->bit_rate		= bitrate_;
    ctx_->thread_count	= 1;
    ctx_->time_base = audio_tb;
    //Allow experimental codecs
//    ctx_->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    if (avcodec_open2(ctx_, codec_, NULL) < 0) {
        LogError( "AAC: could not open codec");
        return false;
    }
    frame_byte_size_ = av_get_bytes_per_sample(ctx_->sample_fmt)
                       * ctx_->ch_layout.nb_channels * ctx_->frame_size;
    //Create frame
    frame_ = av_frame_alloc();
    //Set defaults
    frame_->nb_samples     = ctx_->frame_size;
    frame_->format         = ctx_->sample_fmt;
    frame_->ch_layout = ctx_->ch_layout;
    frame_->sample_rate    = ctx_->sample_rate;
    //分配data buf
    ret = av_frame_get_buffer(frame_, 0);
    if(ret < 0) {
        LogError("av_frame_get_buffer failed");
    }
    LogInfo( "AAC: Encoder open with frame sample size: %d",ctx_->frame_size);
    return true;
}
