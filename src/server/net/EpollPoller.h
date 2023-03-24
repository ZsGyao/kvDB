/**
  ******************************************************************************
  * @file           : EpollPoller.h
  * @author         : zgys
  * @brief          : 管理channel和epoll，负责将channel注册，更新，删除到epoll
  * @attention      : None
  * @date           : 23-3-24
  ******************************************************************************
  */


#ifndef KVDB_EPOLLPOLLER_H
#define KVDB_EPOLLPOLLER_H

#include <vector>
#include <map>
#include "../comm/Noncopyable.h"
#include "../comm/Timestamp.h"

namespace kvDB {

    class Channel;
    class EventLoop;

    class EpollPoller : Noncopyable {
    public:
        using ChannelList = std::vector<Channel*>;

        explicit EpollPoller(EventLoop* loop);
        ~EpollPoller();

        /* 将epoll_wait返回的事件放入成员 events_ 中, 将fd对应的channel从channels_找出，加入activeChannels */
        Timestamp poll(int timeoutMs, ChannelList* activeChannels);

        /* 更新channel到epoll */
        void updateChannel(Channel* channel);

        /* 从Poller中删除channel，只有channel中的fd不监听任何事件才可以移除 */
        void removeChannel(Channel* channel);

        /* 断言是否在循环的线程中 */
        void assertInLoopThread();

    private:
        void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

        /* 将channel中fd监听的events 按 operator(DEL or MOD) 操作epoll */
        bool update(int operation, Channel* channel) const;

        static const int kInitEventListSize = 16;
        using EventList = std::vector<struct epoll_event>;
        using ChannelMap = std::map<int, Channel*>;

    private:
        EventLoop* ownerLoop_;
        int        epollfd_;
        // 缓存epoll_event的数组， 存放epoll_wait返回的事件
        EventList  events_;
        // fd到Channel的映射
        ChannelMap channels_;
    };

}

#endif //KVDB_EPOLLPOLLER_H
