/**
  ******************************************************************************
  * @file           : Socket.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#ifndef KVDB_SOCKET_H
#define KVDB_SOCKET_H

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../comm/Noncopyable.h"
#include "InetAddress.h"

namespace kvDB {
    class Socket : Noncopyable {
    public:
        explicit Socket(int sockfd)
                :sockfd_(sockfd){}

        ~Socket();

        int fd() const {
            return sockfd_;
        }

        /* 绑定网络地址 */
        void bindAddress(const InetAddress& localAddr) const;

        /* 监听套接字 */
        void listen() const;
        int accept(InetAddress * peerAddr) const;

        void shutdownWrite() const;

        /* 禁用 Nagle’s Algorithm，
         * Nagle算法。它规定：如果包的大小满足MSS，那么可以立即发送，否则数据会被放到缓冲区，
         * 等到已经发送的包被确认了之后才能继续发送。 */
        void setTcpNoDelay(bool on) const;

        /* 设置地址重用SO_REUSEADDR
         * SO_REUSEADDR用于对TCP套接字处于TIME_WAIT状态下的socket，才可以重复绑定使用 */
        void setReuseAddr(bool on);

        /* 设置端口重用SO_REUSEPORT  SO_REUSEPORT允许多个线程/进程，绑定在同一个端口上
         * 多个socket可以同时bind同一个tcp/udp端口（ip+port组合）。同时内核保证多个这样的socket的负载均衡 */
        void setReusePort(bool on);

        /* 设置Tcp层的心跳包 */
        void setKeepAlive(bool on);

    private:
        const int sockfd_;
    };

    int createNonblockingOrDie();
    void setNonBlockAndCloseOnExec(int sockfd);
    int connect(int sockfd,const sockaddr_in & addr);
    void bind(int sockfd,const sockaddr_in & addr);
    bool isSelfConnect(int sockfd);
    void close(int sockfd);
    sockaddr* sockaddr_cast(sockaddr_in* addr);
    const sockaddr* sockaddr_cast(const sockaddr_in* addr);
    sockaddr_in getLocalAddr(int sockfd);
    sockaddr_in getPeerAddr(int sockfd);
    int getSocketError(int sockfd);

}




#endif //KVDB_SOCKET_H
