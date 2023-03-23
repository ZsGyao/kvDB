/**
  ******************************************************************************
  * @file           : Channel.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#ifndef KVDB_CHANNEL_H
#define KVDB_CHANNEL_H

#include <functional>
#include <memory>
#include "../comm/Noncopyable.h"
#include "../comm/Timestamp.h"

namespace kvDB {

    class EventLoop;

    class Channel : public Noncopyable {
    public:
        using EventCallback = std::function<void()>;
        using ReadEventCallback = std::function<void(Timestamp)>;

        Channel(EventLoop *loop, int fd);
        ~Channel();

        void handleEvent(Timestamp receiveTime);

        void setReadCallback(const ReadEventCallback &cb) { readCallback_ = cb; }
        void setWriteCallback(const EventCallback &cb) { writeCallback_ = cb; }
        void setErrorCallback(const EventCallback &cb) { errorCallback_ = cb; }
        void setCloseCallback(const EventCallback &cb) { closeCallback_ = cb; }

        int fd() const { return fd_; }
        int events() const { return events_; }
        void set_revents(int revt) { revents_ = revt; }
        bool isNoneEvent() const { return events_ == kNoneEvent; }
        void tie(const std::shared_ptr<void> &);

        void enableReading();
        void enableWriting();
        void disableWriting();
        void disableAll();

        bool isWriting() const { return events_ & kWriteEvent; }
        // for Poller
        int index() { return index_; }
        void set_index(int idx) { index_ = idx; }

        EventLoop *ownerLoop() { return loop_; }

    private:
        void update();
        void handleEventWithGuard(Timestamp receiveTime);

        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

        EventLoop *loop_;
        const int fd_;
        int events_;
        int revents_;
        int index_;
        // for TcpConnection
        std::weak_ptr<void> tie_;
        bool tied_;

        ReadEventCallback readCallback_;
        EventCallback     writeCallback_;
        EventCallback     errorCallback_;
        EventCallback     closeCallback_;
    };
}



#endif //KVDB_CHANNEL_H
