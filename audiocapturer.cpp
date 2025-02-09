#include "audiocapturer.h"
#include "logserve.h"
#include <QDebug>
AudioCapturer::AudioCapturer()
{

}

bool AudioCapturer::Init(const Properties &properties,AVChannelLayout audio_enc_channel_layout)
{
    audio_enc_sample_rate_ = properties.GetProperty("audio_enc_sample_rate", 0);
    audio_enc_sample_fmt_ = properties.GetProperty("audio_enc_sample_fmt", 0);
    audio_enc_channle_layout_ = audio_enc_channel_layout;
    audio_enc_frame_size_ = properties.GetProperty("audio_enc_frame_size", 0);
    device_name_ = properties.GetProperty("audio_device_name", "");
    // 去噪

    //
    if(audio_enc_sample_rate_ == 0
        || audio_enc_sample_fmt_ == 0
        || audio_enc_channle_layout_.nb_channels == 0
        || audio_enc_frame_size_ == 0)
        LogError("AudioCapturer init properties have null parameter, please check it");

    if(device_name_.empty())
        LogError("AudioCapturer init failed, device_name is empty");

// OS
#ifdef Q_OS_WIN
    // Windows:dshow Linux:alsa
    AVInputFormat *inputfmt =(AVInputFormat*) av_find_input_format("dshow");
    ifmt_ctx_ = avformat_alloc_context();
    //设置音频参数
    AVDictionary*opts = nullptr;

    std::string sample_rate_str = std::to_string(capture_sample_rate_);
    const char* capture_sample_rate = sample_rate_str.c_str();
    av_dict_set(&opts,"sample_rate",capture_sample_rate,0);
    //av_dict_set(&opts, "audio_buffer_size", "10", 0); // 设置10ms， 每次降噪需要10ms的数据

    if(avformat_open_input(&ifmt_ctx_,device_name_.data(),inputfmt,&opts) != 0)
    {
        LogError("open input stream audio %s is failed!",device_name_.data());
        return false;
    }
#endif
    // 找音频流
    for(int i = 0 ;i < ifmt_ctx_->nb_streams;i++)
    {
        if(ifmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audio_stream_ = i;
            break;
        }
    }

    if(audio_stream_ == -1)
    {
        LogError("find audio_stream is failed!");
        return false;
    }

    codec_ctx_ = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(codec_ctx_,ifmt_ctx_->streams[audio_stream_]->codecpar);
    AVCodec*codec = (AVCodec*)avcodec_find_decoder(codec_ctx_->codec_id);
    if(!codec)
    {
        LogError("Could not avcodec_find_encoder");
        return false;
    }
    //打开编码器
    if(avcodec_open2(codec_ctx_,codec,nullptr))
    {
        LogError("avcodec_open2 is failed!");
        return false;
    }

    // 初始化重采样器

    if(initResample() == false)
    {
        LogError("initResample failed!");
        return false;
    }

    // 初始化解码帧
    encode_frame_ = av_frame_alloc();
    encode_frame_->format = audio_enc_sample_fmt_;
    encode_frame_->ch_layout = audio_enc_channel_layout;
    encode_frame_->sample_rate = audio_enc_sample_rate_;
    encode_frame_->nb_samples = audio_enc_frame_size_;  // aac
    int ret = av_frame_get_buffer(encode_frame_, 0);
    if (ret < 0) {
        LogError("Error allocating an audio buffer");
        return false;
    }

    capture_frame_ = av_frame_alloc();
    return true;
}

bool AudioCapturer::Start()
{
    LogInfo("Start AudioCapturer");
    loop_thread_ = std::make_unique<std::thread>([this](){this->loop();});
    return true;
}

bool AudioCapturer::initResample()
{
    // 输入的采样格式
    capture_sample_fmt_ = codec_ctx_->sample_fmt;
    // 输入的采样率
    capture_sample_rate_ = codec_ctx_->sample_rate;
    // 输入的声道布局
    capture_channel_layout_ = codec_ctx_->ch_layout;
    if (capture_channel_layout_.nb_channels <= 0) {
        if (codec_ctx_->ch_layout.nb_channels == 2) {
            capture_channel_layout_.u.mask = AV_CH_LAYOUT_STEREO;
        } else {
            capture_channel_layout_.u.mask = AV_CH_LAYOUT_MONO;
        }
    }

    // 重采样实例
    AudioResample::ResampleParams resample_params;
    // srs
    resample_params.srcChannelLayout = capture_channel_layout_;
    resample_params.srcSampleFmt = capture_sample_fmt_;
    resample_params.srcSampleRate = capture_sample_rate_;
    // dst
    resample_params.dstChannelLayout = capture_channel_layout_;
    resample_params.dstChannelLayout.u.mask = AV_CH_LAYOUT_STEREO;
    resample_params.dstSampleFmt = audio_enc_sample_fmt_;
    resample_params.dstSampleRate = audio_enc_sample_rate_;
    //qDebug()<<resample_params.dstChannelLayout.nb_channels;
    capture2encode_resmaple_ = std::make_unique<AudioResample>();
    if(capture2encode_resmaple_->Init(resample_params) == false)
    {
        LogError("capture2encode_resmaple_ init is failed!");
        return false;
    }
    return true;
}

void AudioCapturer::loop()
{

    AVPacket pkt;
    int ret = -1;
    while(true)
    {
        ret = av_read_frame(ifmt_ctx_,&pkt);
        if(ret <0)
        {
            LogWarn("read failed!");
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            continue;
        }

        if(pkt.stream_index != audio_stream_){
            LogWarn("other stream_index:%d ",pkt.stream_index);
            av_packet_unref(&pkt);
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            continue;
        }

        // 开始解码
        ret = avcodec_send_packet(codec_ctx_,&pkt);
        if(ret != 0)
        {
            LogError("input decoder pkt is failed!");
            av_packet_unref(&pkt);
            continue;
        }

        while(avcodec_receive_frame(codec_ctx_,capture_frame_) == 0)
        {
            // 重采样
            Resample_Process();
            //callback_get_frame_(capture_frame_);
        }
        av_packet_unref(&pkt);
    }
    LogInfo("audioCapture loop thread is end!");
    return;
}

///
/// \brief AudioCapturer::Resample_Process
/// \return
/// 对解码后的数据进行重采样
bool AudioCapturer::Resample_Process()
{
    return true;
}
