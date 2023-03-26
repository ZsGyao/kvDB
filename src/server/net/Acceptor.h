/**
  ******************************************************************************
  * @file           : Acceptor.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-26
  ******************************************************************************
  */


#ifndef KVDB_ACCEPTOR_H
#define KVDB_ACCEPTOR_H

#include <functional>
#include "../comm/Noncopyable.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"

namespace kvDB {
    class Acceptor : Noncopyable {
    public:
        using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;

        Acceptor(EventLoop* loop, const InetAddress& listenAddr);

        /* 设置新的Tcp连接执行的回调 */
        void setNewConnectionCallback(const NewConnectionCallback& cb) {
            newConnectionCallback_ = cb;
        }

        /* 监听Acceptor实例中Socket类的fd，设置fd关心读事件 */
        void listen();

        /* Acceptor是否已经监听了f */
        bool listening() const {
            return listening_;
        }

    private:
        void handleRead();

        EventLoop* loop_;          // 运行的循环
        Socket acceptSocket_;      // 网络socket
        Channel acceptChannel_;    // fd监听

        NewConnectionCallback newConnectionCallback_;  // 新连接到来执行的回调
        bool listening_;                 // 是否已经监听了Socket中的fd，fd关心读事件
        int idleFd_;                     // 防文件描述符耗尽
    };
}

#endif //KVDB_ACCEPTOR_H
