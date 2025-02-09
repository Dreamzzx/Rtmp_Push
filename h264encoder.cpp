#include "h264encoder.h"

H264Encoder::H264Encoder()
{

}

bool H264Encoder::Init(Properties properties, AVRational video_tb)
{
    width_ = properties.GetProperty("width", 0);
    if(width_ == 0 || width_ % 2 != 0) {
        LogError("width: %d", width_);
        return false;
    }
    height_ = properties.GetProperty("height", 0);
    if(height_ == 0 || height_ % 2 != 0) {
        LogError("height_: %d", height_);
        return false;
    }
    fps_ = properties.GetProperty("fps", 25);
    b_frames_ = properties.GetProperty("b_frames", 0);
    bitrate_ = properties.GetProperty("bitrate", 500 * 1024);
    gop_ = properties.GetProperty("gop", fps_);

    // 创建编码器
    codec_ = (AVCodec*)avcodec_find_decoder(AV_CODEC_ID_H264);
    if(codec_ == NULL)
    {
        LogError("video codec_ find failed!");
        return false;
    }

    ctx_ = avcodec_alloc_context3(codec_);
    if(!ctx_)
    {
        LogError("alloc ctx is failed!");
        return false;
    }
    //Param that must set
    //最大和最小量化系数，取值范围为0~51。
    ctx_->qmin = 10;
    ctx_->qmax = 31;
    //编码后的视频帧大小，以像素为单位。
    ctx_->width = width_;
    ctx_->height = height_;
    //编码后的码率：值越大越清晰，值越小越流畅。
    ctx_->bit_rate = bitrate_;
    //每20帧插入一个I帧
    ctx_->gop_size = gop_;
    //帧率的基本单位，time_base.num为时间线分子，time_base.den为时间线分母，帧率=分子/分母。
    ctx_->time_base  = video_tb;
    ctx_->framerate.num = fps_;
    ctx_->framerate.den = 1;
    //图像色彩空间的格式，采用什么样的色彩空间来表明一个像素点。
    ctx_->pix_fmt = AV_PIX_FMT_YUV420P;
    //编码器编码的数据类型
    ctx_->codec_type = AVMEDIA_TYPE_VIDEO;
    //Optional Param
    //两个非B帧之间允许出现多少个B帧数，设置0表示不使用B帧，没有编码延时。B帧越多，压缩率越高。
    ctx_->max_b_frames = b_frames_;
    if (ctx_->codec_id == AV_CODEC_ID_H264) {
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
    }
    if (ctx_->codec_id == AV_CODEC_ID_H265) {
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zero-latency", 0);
    }
    ctx_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;    //extradata拷贝 sps pps
    if(avcodec_open2(ctx_,codec_,&param) < 0)
    {
        LogError("open avcodec is failed!");
        return false;
    }

    //读取sps pps信息
    if(ctx_->extradata)
    {
        LogInfo("ctx_->extradata size :%d",ctx_->extradata_size);
        //  7byte     8byte
        // 初始码 sps 初始码 pps
        // 00 00 00 1 sps 00 00 00 01 pps
        uint8_t*sps = ctx_->extradata+4; //直接到数据
        int sps_len = 0;
        uint8_t*pps = NULL;
        int pps_len = 0;
        uint8_t* data = ctx_->extradata+4;
        for(int i = 0;i<ctx_->extradata_size -4;i++)
        {
            if(0 == data[i]&&0 == data[i+1]&&0 == data[i+2]&&1 == data[i+3])
            {
                pps = &data[i+4];
                break;
            }
        }
        sps_len = int(pps - sps)-4; //4 为初始码
        pps_len = ctx_->extradata_size - 4*2-sps_len;
        sps_.append(sps,sps+sps_len);
        pps_.append(pps,pps+pps_len);
    }

    return true;
}
