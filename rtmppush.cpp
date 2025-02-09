#include "rtmppush.h"

RtmpPush::RtmpPush():RtmpBase(),NaluLoop(30)
{
    LogInfo("create RtmpPush");
}

RtmpPush::~RtmpPush()
{

}

void RtmpPush::Post(int what, void *data,bool flush)
{
    LoopMessage *msg = new LoopMessage();
    msg->what = what;
    msg->obj = (DataBase*)data;
    msg->quit = false;
    addmsg(msg,flush);
}

void RtmpPush::handle(int what, void *data)
{
    // 首先检查是否已断开连接, 如果断开则尝试重新连接
    if(!IsConnect()) {
        LogWarn(u8"开始断线重连");
        if(!Connect()) {
            LogWarn(u8"重连失败");
            delete data;
            return;
        }
    }

    // 根据数据包类型, 执行不同的处理逻辑
    switch(what) {
    case RTMP_BODY_METADATA: {
//        // 处理元数据包
//        if(!is_first_metadata_) {
//            is_first_metadata_ = true;
//            LOG(INFO) << AVPublishTime::GetInstance()->getMetadataTag() << ":t" << AVPublishTime::GetInstance()->getCurrenTime();
//        }
//        FLVMetadataMsg *metadata = (FLVMetadataMsg*)data;
//        if(!SendMetadata(metadata)) {
//            LOG(ERROR) << "SendMetadata failed";
//        }
//        delete metadata;
//        break;
    }
    case RTMP_BODY_VID_CONFIG: {
//        // 处理视频序列头数据包
//        if(!is_first_video_sequence_) {
//            is_first_video_sequence_ = true;
//            LOG(INFO) << AVPublishTime::GetInstance()->getAvcHeaderTag() << ":t" << AVPublishTime::GetInstance()->getCurrenTime();
//        }
//        VideoSequenceHeaderMsg *vid_cfg_msg = (VideoSequenceHeaderMsg*)data;
//        if(!sendH264SequenceHeader(vid_cfg_msg)) {
//            LOG(ERROR) << "sendH264SequenceHeader failed";
//        }
//        delete vid_cfg_msg;
//        break;
    }
    case RTMP_BODY_VID_RAW: {
//        // 处理视频原始数据包
//        if(!is_first_video_frame_) {
//            is_first_video_frame_ = true;
//            LOG(INFO) << AVPublishTime::GetInstance()->getAvcFrameTag() << ":t" << AVPublishTime::GetInstance()->getCurrenTime();
//        }
//        NaluStruct* nalu = (NaluStruct*)data;
//        if(sendH264Packet((char*)nalu->data, nalu->size, (nalu->type == 0x05) ? true : false, nalu->pts)) {
//            //LOG(INFO) << "send pack ok";
//        } else {
//            LOG(WARNING) << "at handle send h264 pack failed";
//        }
//        delete nalu;
//        break;
    }
    case RTMP_BODY_AUD_SPEC: {
        // 处理音频序列头数据包
        if(!is_first_audio_sequence_) {
            is_first_audio_sequence_ = true;
            //LogInfo("%s,%s",AVPublishTime::GetInstance()->getAacHeaderTag(),AVPublishTime::GetInstance()->getCurrenTime());
        }
        //flv的audio frame type
        AudioSpecMsg* audio_spec = (AudioSpecMsg*)data;
        uint8_t aac_spec_[4];//data aac header
        aac_spec_[0] = 0xAF;    //1010 1111
        aac_spec_[1] = 0x0;     // 0 = aac sequence header
        //根据audio参数，转化成aac sequence header并写入
        AACRTMPPackager::GetAudioSpecificConfig(&aac_spec_[2], audio_spec->profile_, audio_spec->sample_rate_, audio_spec->channels_);
        SendAudioSpecificConfig((char *)aac_spec_, 4);
        break;
    }
    case RTMP_BODY_AUD_RAW: {
        // 处理音频原始数据包
//        if(!is_first_audio_frame_) {
//            is_first_audio_frame_ = true;
//            LOG(INFO) << AVPublishTime::GetInstance()->getAacDataTag() << ":t" << AVPublishTime::GetInstance()->getCurrenTime();
//        }
//        AudioRawMsg* audio_raw = (AudioRawMsg*)data;
//        if(sendPacket(RTMP_PACKET_TYPE_AUDIO, (unsigned char*)audio_raw->data, audio_raw->size, audio_raw->pts)) {
//        } else {
//            LOG(INFO) << "at handle send audio pack failed";
//        }
//        delete audio_raw;
        break;
    }
    default:
        break;
    }
}

void RtmpPush::addmsg(LoopMessage *msg, bool flush)
{
    mtx.lock();
    if(flush || msg_queue_.size() > deq_msg_size_)
    {
        LogWarn("flush msg deque");
        while(msg_queue_.size() > 0)
        {
            LoopMessage * tmp_msg = msg_queue_.front();
            msg_queue_.pop_front();
            delete tmp_msg->obj;
            delete tmp_msg;
        }
    }
    msg_queue_.push_back(msg);
    mtx.unlock();
}

/**
 * @brief 发送音频编码器的配置信息 设置messageheader
 * https://blog.csdn.net/weixin_50873490/article/details/143110186?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522a7826462a78d8e12f4ab722b2da3d0ac%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fblog.%2522%257D&request_id=a7826462a78d8e12f4ab722b2da3d0ac&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~blog~first_rank_ecpm_v1~rank_v31_ecpm-1-143110186-null-null.nonecase&utm_term=flv&spm=1018.2226.3001.4450
 * @param data 音频编码器的配置数据
 * @param length 配置数据的长度
 * @return 发送是否成功
 */
bool RtmpPush::SendAudioSpecificConfig(char *data, int length)
{
    if(data == NULL)
    {
        return false;
    }

    RTMPPacket packet;
    RTMPPacket_Reset(&packet);
    RTMPPacket_Alloc(&packet,4);

    packet.m_body[0] = data[0];
    packet.m_body[1] = data[1];
    packet.m_body[2] = data[2];
    packet.m_body[3] = data[3];

    // 设置 RTMPPacket 的其他属性
    packet.m_headerType  = RTMP_PACKET_SIZE_LARGE;
    packet.m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet.m_nChannel   = RTMP_AUDIO_CHANNEL;
    packet.m_nTimeStamp = 0;
    packet.m_nInfoField2 = rtmp_->m_stream_id;
    packet.m_nBodySize  = 4;

    // 调用 RTMP_SendPacket() 函数发送 RTMPPacket
    int nRet = RTMP_SendPacket(rtmp_, &packet, 0);
    if (nRet != TRUE) {
        LogError("RTMP_SendPacket fail: %d",nRet);
    }

    // 释放 RTMPPacket 占用的内存
    RTMPPacket_Free(&packet);

    // 返回发送结果
    return (nRet == TRUE ? true : false);
}
