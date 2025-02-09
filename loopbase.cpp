#include "loopbase.h"

LoopBase::LoopBase(const int deque_max_size)
    :deque_max_size_(deque_max_size)
{
    LogInfo("Loop create");
    running_ = true;
    work_ = new std::thread(StartLoop,this);
}

void LoopBase::Post(int what, void *data,bool flush)
{
    LoopMessage *msg = new LoopMessage();
    msg->what = what;
    msg->obj = (DataBase*)data;
    msg->quit = false;
    addmsg(msg,flush);
}

void LoopBase::handle(int what, void *data)
{}

void LoopBase::StartLoop(void *p)
{
    LogInfo("StartLoop");
    ((LoopBase*)p)->loop();
}

void LoopBase::loop()
{
    LogInfo("into loop");
    LoopMessage *msg;
    while(true) {
        mtx_.lock();
        int size = msg_queue_.size();
        if(size > 0)
        {
            msg = msg_queue_.front();
            msg_queue_.pop_front();
            mtx_.unlock();
            //quit 退出
            if (msg->quit)
            {
                break;
            }
            if(msg->what == 1)
            {
                //                LOG(INFO) << "processing into msg " <<  msg->what << ", size: " << size << ", t:%u" << AVPublishTime::GetInstance()->getCurrenTime();
            }

            handle(msg->what, msg->obj);


            if(msg->what == 1) {
                //                LOG(INFO) << "processing leave msg " <<  msg->what << ", size: " << size << ", t:%u" << AVPublishTime::GetInstance()->getCurrenTime();
            }
            delete msg;
        }
        else
        {
            mtx_.unlock();
            //            if(msg->what == 1)
            //            LOG(INFO) << "sleep msg, t: " << AVPublishTime::GetInstance()->getCurrenTime();
            //head_data_available_->wait();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    delete msg->obj;
    delete msg;
    while(msg_queue_.size() > 0) {
        msg = msg_queue_.front();
        msg_queue_.pop_front();
        delete msg->obj;
        delete msg;
    }
}
