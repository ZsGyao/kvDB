/**
  ******************************************************************************
  * @file           : EventLoop.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#include "EventLoop.h"
#include "Channel.h"
#include "../comm/Logger.h"
#include <cassert>

namespace kvDB {

    /* 当前线程中的EventLoop，每个线程只有一个EventLoop */
    thread_local EventLoop* t_loopInThread = nullptr;

    // 定义Poller IO复用接口的默认超时时间
    const int kPollTimeMs = 10000;

    EventLoop::EventLoop()
            : looping_(false),
              quit_(false),
              threadId_(std::this_thread::get_id()),
              poller_(new kvDB::EpollPoller(this)){
        if (t_loopInThread) {
            LOG_FATAL("Another EventLoop %p existed in this thread.\n", t_loopInThread);
        } else {
            t_loopInThread = this;
        }
    }

    EventLoop::~EventLoop() {
        assert(!looping_);
        t_loopInThread = nullptr;
    }

    void EventLoop::abortNotInLoopThread() {
        LOG_FATAL("EventLoop:abortNotInLoopThread-EventLoop %p was created in "
                  "threadId = %lld, current thread id = %lld",
                  this, threadId_, std::this_thread::get_id());
    }

    void EventLoop::loop() {
        assert(!looping_);
        assertInLoopThread();
        looping_ = true;
        quit_ = false;

        while (!quit_) {
            activeChannels_.clear();
            epollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
            for (Channel* channel : activeChannels_) {
                channel->handleEvent(epollReturnTime_);
            }
        }
        looping_ = false;
    }

    void EventLoop::runInLoop(const Functor &cb) {
        cb();
    }

    void EventLoop::quit() { quit_ = true; }

    void EventLoop::updateChannel(Channel *channel) {
        assert(channel->ownerLoop() == this);
        assertInLoopThread();
        poller_->updateChannel(channel);
    }

    void EventLoop::removeChannel(Channel *channel) {
        assert(channel->ownerLoop() == this);
        assertInLoopThread();
        poller_->removeChannel(channel);
    }

}