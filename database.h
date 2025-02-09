#ifndef DATABASE_H
#define DATABASE_H

#include "globalhelp.h"

enum RTMP_BODY_TYPE {
    RTMP_BODY_METADATA, // metadata
    RTMP_BODY_AUD_RAW,  // 纯raw data
    RTMP_BODY_AUD_SPEC, // AudioSpecificConfig
    RTMP_BODY_VID_RAW,  // raw data
    RTMP_BODY_VID_CONFIG// H264Configuration
};

class DataBase
{
public:
    DataBase();
};

typedef struct LoopMessage{
    int what;
    DataBase*obj;
    bool quit;
}LoopMessage;

enum RTMPChannel
{
    RTMP_NETWORK_CHANNEL = 2,   ///< channel for network-related messages (bandwidth report, ping, etc)
    RTMP_SYSTEM_CHANNEL,        ///< channel for sending server control messages
    RTMP_AUDIO_CHANNEL,         ///< channel for audio data
    RTMP_VIDEO_CHANNEL   = 6,   ///< channel for video data
    RTMP_SOURCE_CHANNEL  = 8,   ///< channel for a/v invokes
};
class RTMPPackager
{
public:
    virtual void Pack(RTMPPacket *packet, char* buf, const char* data, int length) const = 0;
    virtual void Metadata(RTMPPacket *packet, char* buf, const char* data, int length) const = 0;
};

class AACRTMPPackager : public RTMPPackager
{
public:
    static const int AAC_MAIN	= 1;
    static const int AAC_LC		= 2;
    static const int AAC_SSR	= 3;
    virtual void Pack(RTMPPacket *packet, char* buf, const char* data, int length)  const override;
    virtual void Metadata(RTMPPacket *packet, char* buf, const char* data, int length) const override;

    static int GetAudioSpecificConfig(uint8_t* data, const uint32_t profile, const uint32_t samplerate, const uint32_t channel_num);
    static void PacketADTSHeader(uint8_t *packet, int packet_len, int profile, int freq_idx, int channel_config);
};

class AudioSpecMsg :public DataBase
{
public:
    AudioSpecMsg(
        uint8_t profile = 2,         // 默认AAC LC
        uint8_t channel_num = 2,     // 默认立体声
        uint32_t samplerate = 48000  // 默认48kHz
        ) :
        profile_(profile),
        channels_(channel_num),
        sample_rate_(samplerate),
        pts_(0)  // 初始化pts
    {}
    virtual ~AudioSpecMsg() {}
    uint8_t profile_ = 2;   // 2: AAC LC(Low Complexity)
    uint8_t channels_ = 2;  // 默认立体声
    uint32_t sample_rate_ = 48000;  // 默认48kHz
    int64_t pts_ = 0;       // 初始化为0
};

#endif // DATABASE_H
