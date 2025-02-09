#ifndef LOOPBASE_H
#define LOOPBASE_H

#include <deque>
#include <mutex>
#include "database.h"
#include "logserve.h"

///
/// 消息队列循环基类 ， 只需重写虚函数handle和addmsg就可以实现自己的消息队列
///
/// \brief The LoopBase class
///
class LoopBase
{
public:
    LoopBase(const int deque_max_size = 30);

    // 终止队列
    void Stop();

    // 向消息队列中添加消息， flush判断是否清空队列
    void Post(int what, void *data,bool flush);

    virtual void handle(int what,void*data);
private:
    // 添加信息到队列中
    virtual void addmsg(LoopMessage*msg,bool flush) = 0;

    // 循环函数进入接口
    static void StartLoop(void*p);

    // 循环函数主函数
    void loop();

protected:
    std::deque<LoopMessage*> msg_queue_;

    std::mutex mtx_;

    //工作线程
    std::thread *work_;

    // 运行标记
    bool running_;

    int deque_max_size_ = 30;

};

#endif // LOOPBASE_H
