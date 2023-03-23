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
    class Socket : public Noncopyable {
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

        void setTcpNoDelay(bool on) const;
        void setReuseAddr(bool on);
        void setReusePort(bool on);
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
