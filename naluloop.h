#ifndef NALULOOP_H
#define NALULOOP_H
#include "loopbase.h"

class NaluLoop:public LoopBase
{
public:
    NaluLoop(int queue_nalu_len);

    virtual void handle(int what,void*data);
private:

    virtual void addmsg(LoopMessage*msg,bool flush);

    // 处理最大的nalu的数量
    int max_nalu_;
};

#endif // NALULOOP_H
