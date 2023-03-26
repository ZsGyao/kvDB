/**
  ******************************************************************************
  * @file           : Buffer.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#include "Buffer.h"
#include <sys/uio.h>

namespace kvDB {

    const char Buffer::kCRLF[] = "\r\n";

    Buffer::Buffer()
            :buffer_(kCheapPrepend + kInitialSize),
             readerIndex_(kCheapPrepend),
             writerIndex_(kCheapPrepend){

        assert(readableBytes() == 0);
        assert(writableBytes() == kInitialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    /* 使用vector::swap和std::swap，这样就不需要涉及数据拷贝 */
    void Buffer::swap(Buffer& rhs) {
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_,rhs.readerIndex_);
        std::swap(writerIndex_,rhs.writerIndex_);
    }

    ssize_t Buffer::readFd(int fd, int* savedErrno) {
        // saved an ioctl()/FIONREAD call to tell how much to read
        char extrabuf[65536];
        struct iovec vec[2];
        const size_t writable = writableBytes();
        vec[0].iov_base = begin() + writerIndex_;
        vec[0].iov_len = writable;
        vec[1].iov_base = extrabuf;
        vec[1].iov_len = sizeof extrabuf;

        const ssize_t n = readv(fd,vec,2);
        if(n < 0){
            //读报错就记录
            *savedErrno = errno;
        }else if(size_t(n) <= writable){
            //读到的数据小于可写数据空间时，
            //说明数据直接读去到Buffer中
            writerIndex_ += n;
        }else{
            //当读去的数据超过Buffer可写空间时，
            //就用到了栈空间，这时要将栈中的数据
            //存入到Buffer中。
            writerIndex_ = buffer_.size();
            append(extrabuf,n - writable);
        }
        return n;
    }

}