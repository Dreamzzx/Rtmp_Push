#include "pushwork.h"

PushWork::PushWork()
{

}

bool PushWork::Init(const Properties &properties)
{
    // 注册所有可用的音频设备
    avdevice_register_all();

    // 获取可用的音频设别的名称
    audio_device_name_ = properties.GetProperty("audio_device_name","");
    if(audio_device_name_ == "")
    {
        LogError("audio_device_name is null");
        return false;
    }

    // 获取视频设备名称
    video_device_name_ = properties.GetProperty("video_device_name","");
    if(video_device_name_ == "")
    {
        LogError("video_device_name is null");
        return false;
    }

    // 音频参数
    audio_sample_rate_ = properties.GetProperty("audio_sample_rate", 48000);
    audio_bitrate_ = properties.GetProperty("audio_bitrate", 128 * 1024);
    audio_channels_ = properties.GetProperty("audio_channels", 2);

    audio_ch_layout_ = new AVChannelLayout();
    av_channel_layout_default(audio_ch_layout_,audio_channels_);

    // 获取桌面录制属性
    desktop_x_ = properties.GetProperty("desktop_x", 0);
    desktop_y_ = properties.GetProperty("desktop_y", 0);
    desktop_width_ = properties.GetProperty("desktop_width", 1920);
    desktop_height_ = properties.GetProperty("desktop_height", 1080);
    desktop_format_ = properties.GetProperty("desktop_pixel_format", AV_PIX_FMT_YUV420P);
    desktop_fps_ = properties.GetProperty("desktop_fps", 25);

    // 视频参数
    video_width_ = properties.GetProperty("video_width", desktop_width_);
    video_height_ = properties.GetProperty("video_height", desktop_height_);
    video_fps_ = properties.GetProperty("video_fps", desktop_fps_);
    video_gop_ = properties.GetProperty("video_gop", video_fps_);
    video_bitrate_ = properties.GetProperty("video_bitrate", 1024 * 1024);
    video_b_frames_ = properties.GetProperty("video_b_frames", 0);

    // 是否启用rtmp推流
//    if(true)
//    {
//        rtmp_url_ = properties.GetProperty("rtmp_url","");
//        if(rtmp_url_ == "")
//        {
//            LogError("rtmp_url is null");
//            return false;
//        }

//        // 启动RTMP推流器
//        rtmp_push_ = new RtmpPush();
//        if(!rtmp_push_)
//        {
//            LogError("new RtmpPush is failed!");
//            return false;
//        }
//        if(!rtmp_push_->Connect(rtmp_url_))
//        {
//            LogError("RtmpPush connect is failed!");
//            return false;
//        }
//    }

    // 初始化音频解码器
    audio_pkt_ = av_packet_alloc();
    audio_encoder_ = new AACEncoder();
    if (!audio_encoder_)
    {
        LogError("new AACEncoder() failed");
        return false;
    }
    Properties aud_codec_properties;
    aud_codec_properties.SetProperty("sample_rate", audio_sample_rate_);
    aud_codec_properties.SetProperty("channels", audio_channels_);
    aud_codec_properties.SetProperty("bitrate", audio_bitrate_);
    AVRational audio_tb = {1, audio_sample_rate_};
    if (audio_encoder_->Init(aud_codec_properties, audio_tb) == false)
    {
        LogError("AACEncoder Init failed");
        return false;
    }

    // 初始化视频编码器
    video_pkt_ = av_packet_alloc();
    video_encoder_ = new H264Encoder();
    Properties vid_codec_properties;
    vid_codec_properties.SetProperty("width", video_width_);
    vid_codec_properties.SetProperty("height", video_height_);
    vid_codec_properties.SetProperty("fps", video_fps_);
    vid_codec_properties.SetProperty("b_frames", video_b_frames_);
    vid_codec_properties.SetProperty("bitrate", video_bitrate_);
    vid_codec_properties.SetProperty("gop", video_gop_);
    AVRational video_tb = {1, 1000};
    if (video_encoder_->Init(vid_codec_properties, video_tb) == false)
    {
        LogError( "H264Encoder Init failed");
        return false;
    }

    // 初始化音频采集器
    audio_capturer_ = new AudioCapturer();
    Properties aud_cap_properties;
    aud_cap_properties.SetProperty("audio_enc_sample_rate", audio_encoder_->get_sample_rate());
    aud_cap_properties.SetProperty("audio_enc_sample_fmt", audio_encoder_->get_sample_fmt());
    //aud_cap_properties.SetProperty("audio_enc_channel_layout", audio_encoder_->get_channel_layout());
    aud_cap_properties.SetProperty("audio_enc_frame_size", audio_encoder_->get_frame_size());
    aud_cap_properties.SetProperty("audio_device_name", audio_device_name_);
    aud_cap_properties.SetProperty("audio_ns", properties.GetProperty("audio_ns", -1));
    aud_cap_properties.SetProperty("audio_agc", properties.GetProperty("audio_agc", 0));
    if (audio_capturer_->Init(aud_cap_properties,audio_encoder_->get_channel_layout()) == false)
    {
        LogError( "AudioCapturer Init failed");
        return false;
    }
    audio_capturer_->AddCallback(std::bind(&PushWork::AudioFrameCallback, this, std::placeholders::_1));
    if (audio_capturer_->Start() == false)
    {
        LogError("AudioCapturer Start failed");
        return false;
    }

    // 初始化视频采集器

    return true;
}



void PushWork::AddVideoPreviewCallBack(std::function<int (const Frame *)> callback)
{
    video_preview_callback_ = callback;
}

void PushWork::AudioFrameCallback(AVFrame *frame)
{
    if(need_send_audio_spec_config_)
    {
        need_send_audio_spec_config_ = false;
        AudioSpecMsg *aud_spc_msg = new AudioSpecMsg(audio_encoder_->get_profile(),
                                                     audio_encoder_->get_channel_layout().nb_channels,
                                                     audio_encoder_->get_sample_rate());
        aud_spc_msg->pts_ = 0;

    }

    frame->pts = av_rescale_q(frame->pts,capture_time_base_,audio_encoder_->get_time_base());
    int ret = audio_encoder_->Input(frame);
    if(ret < 0)
    {
        LogError("audio_encoder_->Input is failed!");
        //return;
    }
    do{
        ret = audio_encoder_->Output(audio_pkt_);
        if(ret == 0) {
            if(true) {
                pushRtmpAudio(audio_pkt_);
            }
//            if(enable_record_) {
//                pushRecordAudio(audio_packet_);
//            }
            av_packet_unref(audio_pkt_);
            continue;
        } else {
            break;
        }
    }while(ret == 0);

}

bool PushWork::pushRtmpAudio(AVPacket *pkt)
{
    int aac_size = pkt->size;



    return true;
}
