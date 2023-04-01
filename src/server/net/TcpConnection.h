/**
  ******************************************************************************
  * @file           : TcpConnection.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-26
  ******************************************************************************
  */


#ifndef KVDB_TCPCONNECTION_H
#define KVDB_TCPCONNECTION_H

#include <memory>
#include <string>
#include "EventLoop.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "Socket.h"


namespace kvDB {
    class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
    public:
        /**
         * @brief 构造
         * @param loop 此tcp连接实例执行的循环
         * @param name tcp连接名
         * @param sockfd 网络套接字
         * @param localAddr 本地地址
         * @param peerAddr 源地址
         */
        TcpConnection(EventLoop* loop,
                      std::string& name,
                      int sockfd,
                      const InetAddress & localAddr,
                      const InetAddress & peerAddr);

        ~TcpConnection() = default;

        /* 获取Tcp连接的事件循环 */
        EventLoop* getLoop() const { return loop_; }
        /* 获取Tcp连接名称 */
        const std::string& name() const { return name_; }
        /* 获取本地地址 */
        const InetAddress& localAddr() const{ return localAddr_; };
        /* 获取源地址 */
        const InetAddress& peerAddr() const{ return peerAddr_; }
        /* 判断Tcp连接是否是已连接状态 */
        bool connected() const { return state_ == kConnected; }

        void setConnectionCallback(const ConnectionCallback& cb){ connectionCallback_ = cb;}
        void setMessageCallback(const MessageCallback & cb){ messageCallback_ = cb;}
        void setWriteCompletedCallback(const WriteCompleteCallback & cb){ writeCompleteCallback_ = cb;}
        void setCloseCallback(const CloseCallback & cb){ closeCallback_ = cb;}

        /* 当接收了一个新连接后调用.
         * TcpConnection建立完成， 状态置为：kConnected， channel中使用弱引用指向此连接实例
         * channel中的fd关心读事件，执行tcp连接建立的回调*/
        void connectEstablished();

        /* 从TCPServer的map中删除
         * 删除Tcp连接， 状态置为：kDisConnected，取消此fd关心的所有事件，从EventLoop(Reactor)中取消此channel(FdEvent)
         * 执行连接建立的回调*/
        void connectDestroyed();

        /* 发送消息 */
        void send(const std::string& message);

        /* 关闭连接 */
        void shutdown();

        /*设置连接的网络套接字禁用 Nagle’s Algorithm， */
        void setTcpNoDelay(bool on);

        /* 设置连接的网络套接字发生心跳包 */
        void setKeepAlive(bool on);

    private:
        enum StateE{
            kConnecting,     // 正在建立Tcp连接
            kConnected,      // Tcp连接建立完成
            kDisconnecting,  // 正在关闭Tcp连接
            kDisConnected,   // Tcp连接已关闭
        };

        void setState(StateE s) { state_ = s; }
        /* TcpConnection读事件到来时，处理读事件 */
        void handleRead(Timestamp receiveTime);
        /* TcpConnection写事件到来时，处理写事件 */
        void handleWrite();
        /* 处理 TcpConnection关闭 */
        void handleClose();
        /* 处理 TcpConnection 错误 */
        void handleError();

        void sendInLoop(const std::string & message);
        void shutdownInLoop();

        EventLoop* loop_;
        std::string name_;
        StateE state_;
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;
        InetAddress localAddr_;
        InetAddress peerAddr_;

        ConnectionCallback    connectionCallback_;
        MessageCallback       messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;  // 写完成执行的回调
        CloseCallback         closeCallback_;

        Buffer inputBuffer_;
        Buffer outputBuffer_;
    };
}


#endif //KVDB_TCPCONNECTION_H
