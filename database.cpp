#include "database.h"

DataBase::DataBase()
{

}

int AACRTMPPackager::GetAudioSpecificConfig(uint8_t *data, const uint32_t profile, const uint32_t samplerate, const uint32_t channel_num)
{
    return 0;
}

void AACRTMPPackager::PacketADTSHeader(uint8_t *packet, int packet_len, int profile, int freq_idx, int channel_config)
{
    /*int avpriv_mpeg4audio_sample_rates[] = {
    96000, 88200, 64000, 48000, 44100, 32000,
    24000, 22050, 16000, 12000, 11025, 8000, 7350
    };
    channel_configuration: 表示声道数chanCfg
    0: Defined in AOT Specifc Config
    1: 1 channel: front-center
    2: 2 channels: front-left, front-right
    3: 3 channels: front-center, front-left, front-right
    4: 4 channels: front-center, front-left, front-right, back-center
    5: 5 channels: front-center, front-left, front-right, back-left, back-right
    6: 6 channels: front-center, front-left, front-right, back-left, back-right, LFE-channel
    7: 8 channels: front-center, front-left, front-right, side-left, side-right, back-left, back-right, LFE-channel
    8-15: Reserved
    */
    // fill in ADTS data
    packet[0] = (uint8_t)0xFF;
    packet[1] = (uint8_t)0xF9;
    packet[2] = (uint8_t)(((profile - 1) << 6) + (freq_idx << 2) + (channel_config >> 2));
    packet[3] = (uint8_t)(((channel_config & 3) << 6) + (packet_len >> 11));
    packet[4] = (uint8_t)((packet_len & 0x7FF) >> 3);
    packet[5] = (uint8_t)(((packet_len & 7) << 5) + 0x1F);
    packet[6] = (uint8_t)0xFC;
}
