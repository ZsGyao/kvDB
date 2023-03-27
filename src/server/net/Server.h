/**
  ******************************************************************************
  * @file           : Server.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-27
  ******************************************************************************
  */


#ifndef KVDB_SERVER_H
#define KVDB_SERVER_H

#include "EventLoop.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include <map>

namespace kvDB {
    class Server {
    public:
        Server(EventLoop* loop, const InetAddress& listenAddr, std::string name);
        ~Server();

        /* 启动网络服务 */
        void start();

        void setConnectionCallback(const ConnectionCallback& cb){ connectionCallback_ = cb;}
        void setMessageCallback(const MessageCallback& cb){ messageCallback_ = cb;}

    private:
        /* 新建连接(一个IP+Port可以建立多个客户端连接) */
        void newConnection(int sockfd, const InetAddress& peerAddr);
        /* 移除连接(从EventLoop(Reactor)中移除) */
        void removeConnection(const TcpConnectionPtr& conn);
        /* 移除连接(将连接摧毁，从Server的map容器中移除) */
        void removeConnectionInLoop(const TcpConnectionPtr& conn);

        /* first --> tcp连接名  second --> Tcp连接实例 */
        using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

        EventLoop* loop_;
        std::unique_ptr<Acceptor> acceptor_;
        const std::string name_;                 // Server服务实例的名字

        ConnectionCallback connectionCallback_;
        MessageCallback    messageCallback_;

        bool started_;                           // 网络服务是否启动
        int nextConnId_;                         // 下一个Tcp连接的Id
        ConnectionMap connections_;              // 连接名与Tcp连接的映射
    };
}

#endif //KVDB_SERVER_H
