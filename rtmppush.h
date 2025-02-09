#ifndef RTMPPUSH_H
#define RTMPPUSH_H

#include "rtmpbase.h"
#include "naluloop.h"
#include <mutex>
#include <deque>

class RtmpPush :public RtmpBase,public NaluLoop
{
public:
    RtmpPush();
    ~RtmpPush();

    void Post(int what,void* data,bool flush);

    void Loop();

    virtual void handle(int what,void* data);
private:

    void addmsg(LoopMessage *msg ,bool flush);

    bool SendAudioSpecificConfig(char *data, int length);

    std::mutex mtx; 
    // 消息队列
    std::deque<LoopMessage * > msg_queue_;
    int deq_msg_size_ = 30;

    bool is_first_audio_sequence_ = false;
};

#endif // RTMPPUSH_H
