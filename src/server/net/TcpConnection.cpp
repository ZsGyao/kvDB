/**
  ******************************************************************************
  * @file           : TcpConnection.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-26
  ******************************************************************************
  */


#include "TcpConnection.h"
#include "../comm/Logger.h"
#include "Channel.h"

#include <unistd.h>

namespace kvDB {
    void defaultConnectionCallback(const TcpConnectionPtr& conn) {
        LOG_INFO("Server - %s -> %s is %s.\n",
                 conn->peerAddr().toIpPort().c_str(),
                 conn->localAddr().toIpPort().c_str(),
                 (conn->connected() ? "UP" : "DOWN"));
    }

    void defaultMessageCallback(const TcpConnectionPtr&,
                                Buffer* buf,
                                Timestamp) {
        buf->retrieveAll();
    }

    TcpConnection::TcpConnection(EventLoop *loop,
                                 std::string& name,
                                 int sockfd,
                                 const InetAddress& localAddr,
                                 const InetAddress& peerAddr)
            : loop_(loop),
              name_(name),
              state_(kConnecting),
              socket_(new Socket(sockfd)),
              channel_(new Channel(loop, sockfd)),
              localAddr_(localAddr),
              peerAddr_(peerAddr) {

        channel_->setReadCallback(
                std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
        channel_->setWriteCallback(
                std::bind(&TcpConnection::handleWrite, this));
        channel_->setCloseCallback(
                std::bind(&TcpConnection::handleClose, this));
        channel_->setErrorCallback(
                std::bind(&TcpConnection::handleError, this));
    }

    void TcpConnection::connectEstablished() {
        loop_->assertInLoopThread();
        assert(state_ == kConnecting);
        setState(kConnected);
        channel_->tie(shared_from_this());
        channel_->enableReading();

        connectionCallback_(shared_from_this());
    }

    void TcpConnection::connectDestroyed() {
        loop_->assertInLoopThread();
        assert(state_ == kConnected || state_ == kDisconnecting);
        setState(kDisConnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
        loop_->removeChannel(channel_.get());
    }

    void TcpConnection::send(const std::string& message) {
        if (state_ == kConnected) {
            if (loop_->isInLoopThread()) {
                sendInLoop(message);
            } else {
                loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
            }
        }
    }

    void TcpConnection::shutdown() {
        if (state_ == kConnected) {
            setState(kDisconnecting);
            loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
        }
    }

    void TcpConnection::setTcpNoDelay(bool on) {
        socket_->setTcpNoDelay(on);
    }

    void TcpConnection::setKeepAlive(bool on) {
        socket_->setKeepAlive(on);
    }

    void TcpConnection::handleRead(Timestamp receiveTime) {
        int saveErrno = 0;
        /* 将信息从内核缓冲区中读到应用层Buffer */
        ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
        if (n > 0) {
            messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
        } else if (n == 0) {
            /* 内核的TCP程序会进行四次挥手关闭连接，如果应用层正在读取数据：
            ①.如果数据没有读完，会继续读取缓冲区的数据；
            ②.如果读完了，在读的话读函数返回0，表示EOF，TCP连接关闭，读到了socket文件末尾；
            如果应用层在写数据：
            ①.如果内核已经完成了四次挥手，则写函数返回-1，errno=EPIPE,同时整个程序收到一个SIGPIPE信号，该信号默认终止整个程序。
            ②如果内核还没有感知到TCP连接关闭，也就是说对端关闭了连接，但是由于距离过远，当前还不知道对端已经关闭了连接，那么调用写函数返回-1，errno=ECONNRESET,(connection reset by peer),本次调用后内核就会关闭TCP连接，下一次再调用写函数情况如①.
             */
            handleClose();
        } else {
            errno = saveErrno;
            LOG_ERROR("TcpConnection::handleRead error.\n");
            handleError();
        }
    }

    void TcpConnection::handleWrite() {
        loop_->assertInLoopThread();
        if (channel_->isWriting()) {
            ssize_t n = ::write(channel_->fd(),
                                outputBuffer_.peek(),
                                outputBuffer_.readableBytes());
            if (n > 0) {
                outputBuffer_.retrieve(n);
                if (outputBuffer_.readableBytes() == 0) {
                    channel_->disableWriting();

                    if (writeCompleteCallback_) {
                        loop_->runInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                    }
                    if (state_ == kDisconnecting) {
                        shutdownInLoop();
                    }
                } else {
                    LOG_DEBUG("I am going to write more data.");
                }
            } else {
                LOG_FATAL("TcpConnection::handleWrite, but the fd in channel not care about write event");
            }
        } else {
            LOG_DEBUG("Connection is down, no more writing.")
        }
    }

    void TcpConnection::handleClose() {
        loop_->assertInLoopThread();
        LOG_INFO("TcpConnection::handleClose state = %d.\n", state_);
        assert(state_ == kConnected || state_ == kDisconnecting);
        channel_->disableAll();
        closeCallback_(shared_from_this());
    }

    void TcpConnection::handleError() {
        int err = kvDB::getSocketError(channel_->fd());
        LOG_ERROR("TcpConnection::handleError [%s]-SO_ERROR=%d.\n", err);
    }

    void TcpConnection::sendInLoop(const std::string& message) {
        loop_->assertInLoopThread();
        ssize_t nwrote = 0;
        // 发送缓冲区没有数据
        if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
            nwrote = ::write(channel_->fd(), message.data(), message.size());
            if (nwrote >= 0) {
                if (static_cast<size_t>(nwrote) < message.size()) {
                    LOG_DEBUG("I am going to write more data.");
                } else if (writeCompleteCallback_) {
                    loop_->runInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                }
            } else {
                nwrote = 0;
                if (errno != EAGAIN) {
                    LOG_FATAL("TcpConnection::sendInLoop.");
                }
            }
        }
        assert(nwrote >= 0);
        if (static_cast<size_t>(nwrote) < message.size()) {
            outputBuffer_.append(message.data() + nwrote, message.size() - nwrote);
            if (!channel_->isWriting()) {
                channel_->enableWriting();
            }
        }
    }

    void TcpConnection::shutdownInLoop() {
        loop_->assertInLoopThread();
        if (!channel_->isWriting()) {
            socket_->shutdownWrite();
        }
    }

}