/**
  ******************************************************************************
  * @file           : Callbacks.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-26
  ******************************************************************************
  */


#ifndef KVDB_CALLBACKS_H
#define KVDB_CALLBACKS_H

#include <functional>
#include <memory>
#include "../comm/Timestamp.h"
#include "Buffer.h"

namespace kvDB {
    class Buffer;
    class TcpConnection;

    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

    /* 定时器的回调 */
    using TimerCallback = std::function<void()>;
    using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
    /* Tcp连接读事件到来时，从内核取数据到应用层Buffer后执行的回调 */
    using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer* buf, Timestamp)>;
    using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
    using CloseCallback = std::function<void(const TcpConnectionPtr&)>;

    /* 默认连接，一个连接到来执行的回调，打印连接的信息 */
    void defaultConnectionCallback(const TcpConnectionPtr& conn);

    /* 消息到来时，执行回调，将应用层buffer重置 */
    void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);
}

#endif //KVDB_CALLBACKS_H
