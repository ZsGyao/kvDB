/**
  ******************************************************************************
  * @file           : Channel.h
  * @author         : zgys
  * @brief          : 包装fd和event，实际上就是FdEvent，管理fd，fd监听的event， fd的读事件回调，写事件回调，
  *                   错误事件回调， 关闭事件回调
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
#include "EventLoop.h"

namespace kvDB {

    class EventLoop;

    class Channel : Noncopyable {
    public:
        using EventCallback = std::function<void()>;
        using ReadEventCallback = std::function<void(Timestamp)>;

        Channel(EventLoop* loop, int fd);
        ~Channel();

        /* 处理事件 */
        void handleEvent(Timestamp receiveTime);

        void setReadCallback(const ReadEventCallback& cb) { readCallback_ = cb; }
        void setWriteCallback(const EventCallback& cb) { writeCallback_ = cb; }
        void setErrorCallback(const EventCallback& cb) { errorCallback_ = cb; }
        void setCloseCallback(const EventCallback& cb) { closeCallback_ = cb; }

        int fd() const { return fd_; }
        int events() const { return events_; }
        void set_revents(int revt) { revents_ = revt; }

        /* 判断此fd是不是没有监听任何事件 */
        bool isNoneEvent() const { return events_ == kNoneEvent; }

        /* 为了防止TcpConnection在运行时，链接被释放掉，用一个弱引用指向它，
         * 一旦使用时， .lock 变成shared_ptr，就不会在使用时释放 */
        void tie(const std::shared_ptr<void> &);

        /* 添加读事件 */
        void enableReading();

        /* 添加写事件 */
        void enableWriting();

        /* 取消写事件 */
        void disableWriting();

        /* 取消所有事件 */
        void disableAll();

        /* 是否监听了写事件 */
        bool isWriting() const { return events_ & kWriteEvent; }

        // for Poller
        int index() { return index_; }
        void set_index(int idx) { index_ = idx; }

        EventLoop* ownerLoop() { return loop_; }

    private:
        /* 更新事件循环中的channel */
        void update();

        /* 执行fd事件的回调 */
        void handleEventWithGuard(Timestamp receiveTime);

    private:
        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

        EventLoop*          loop_;
        const int           fd_;           // fd的文件描述符
        int                 events_;       // fd监听的事件
        int                 revents_;
        int                 index_;
        // for TcpConnection
        std::weak_ptr<void> tie_;          // 绑定TcpConnection
        bool                tied_;         // 是否绑定了TcpConnection

        ReadEventCallback readCallback_;   // fd 读事件到来时执行的回调
        EventCallback     writeCallback_;  // fd 写事件到来时执行的回调
        EventCallback     errorCallback_;  // fd 错误执行的回调
        EventCallback     closeCallback_;  // fd 关闭执行的回调
    };
}



#endif //KVDB_CHANNEL_H
