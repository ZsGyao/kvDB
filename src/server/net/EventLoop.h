/**
  ******************************************************************************
  * @file           : EventLoop.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#ifndef KVDB_EVENTLOOP_H
#define KVDB_EVENTLOOP_H

#include <atomic>
#include <memory>
#include <functional>
#include <thread>
#include <vector>
#include "../comm/Timestamp.h"
#include "EpollPoller.h"

class EpollPoller;
class Channel;

namespace kvDB {
    class EventLoop {
    public:
        using Functor = std::function<void()>;

        EventLoop();
        ~EventLoop();

        /* loop循环, 运行一个死循环.
         * 必须在当前对象的创建线程中运行.
         */
        void loop();

        /*
         * 退出loop循环.
         * 如果通过原始指针(raw pointer)调用, 不是100%线程安全;
         * 为了100%安全, 最好通过shared_ptr<EventLoop>调用
         */
        void quit();

        void assertInLoopThread(){
            if(!isInLoopThread()){
                abortNotInLoopThread();
            }
        }
        bool isInLoopThread() const{
            return threadId_ == std::this_thread::get_id();
        }

        /*
         * 在loop线程中, 立即运行回调cb.
         * 如果没在loop线程, 就会唤醒loop, (排队)运行回调cb.
         * 如果用户在同一个loop线程, cb会在该函数内运行; 否则， 会在loop线程中排队运行.
         * 因此, 在其他线程中调用该函数是安全的.
         */
        void runInLoop(const Functor& cb);

        void updateChannel(Channel * channel);
        void removeChannel(Channel * channel);

    private:
        void abortNotInLoopThread();

    private:
        using ChannelList = std::vector<Channel*>;

    private:
        std::atomic_bool      looping_;                  // 是否正在循环
        std::atomic_bool      quit_;                     // 是否离开正在循环的线程EventLoop
        const std::thread::id threadId_;
        Timestamp             epollReturnTime_;          // epoll返回时间

        std::unique_ptr<EpollPoller>  poller_;
        ChannelList                   activeChannels_;   // 活跃的channel -> fd
    };
}


#endif //KVDB_EVENTLOOP_H
