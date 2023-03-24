/**
  ******************************************************************************
  * @file           : Channel.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#include "Channel.h"
#include <sys/epoll.h>

namespace kvDB {
    const int Channel::kNoneEvent = 0;
    const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
    const int Channel::kWriteEvent = EPOLLOUT;

    Channel::Channel(EventLoop *loop, int fd)
            : loop_(loop),
              fd_(fd),
              events_(0),
              revents_(0),
              index_(-1),
              tied_(false) {

    }

    Channel::~Channel() = default;

    void Channel::tie(const std::shared_ptr<void>& obj) {
        tie_ = obj;
        tied_ = true;
    }

    void Channel::update() {
        loop_->updateChannel(this);
    }

    void Channel::handleEvent(Timestamp receiveTime) {
        std::shared_ptr<void> guard;
        if(tied_){
            guard = tie_.lock();
            if(guard){
                handleEventWithGuard(receiveTime);
            }
        }else{
            handleEventWithGuard(receiveTime);
        }
    }

    void Channel::handleEventWithGuard(Timestamp receiveTime) {
        if (revents_ & EPOLLERR) {
            if (errorCallback_) errorCallback_();
        }
        if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
            if (readCallback_) readCallback_(receiveTime);
        }
        if (revents_ & EPOLLOUT) {
            if (writeCallback_) writeCallback_();
        }
    }

    void Channel::enableReading() {
        events_ |= kReadEvent;
        update();
    }

    void Channel::enableWriting() {
        events_ |= kWriteEvent;
        update();
    }

    void Channel::disableWriting() {
        events_ &= ~kWriteEvent;
        update();
    }

    void Channel::disableAll() {
        events_ = kNoneEvent;
        update();
    }
}