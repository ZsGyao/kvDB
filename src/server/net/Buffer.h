/**
  ******************************************************************************
  * @file           : Buffer.h
  * @author         : zgys
  * @brief          : 应用层buffer， 参考Muduo
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#ifndef KVDB_BUFFER_H
#define KVDB_BUFFER_H

#include <cstdio>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>

namespace kvDB {

    /* 应用层缓冲区
                  * +-------------+------------+-------------+
    vector<char>  * | prependable |  readable  |   writable  |
                  * +-------------+------------+-------------+
                  * |             |            |             |
                  * 0         readIndex_   writeIndex_     size
     */
    class Buffer {
    public:
        static const size_t kCheapPrepend = 8;     // 预留空间
        static const size_t kInitialSize  = 1024;  // Buffer初始化大小

        Buffer();
        ~Buffer() = default;

        /* 交换两个应用层buff */
        void swap(Buffer& rhs);

        /* 从内核协议栈的缓存区读取到应用层buffer */
        ssize_t readFd(int fd, int* savedErrno);

        /* buffer中可读的字节数 */
        size_t readableBytes() const { return writerIndex_ - readerIndex_; }

        /* buffer中可写的字节数 */
        size_t writableBytes() const { return buffer_.size() - writerIndex_; }

        /* 前置预留空间大小，一般是0~readerIndex_的范围 */
        size_t prependableBytes() const { return readerIndex_; }

        /* begin(): 指buffer的首地址  return: peek是指可读数据地址首部 */
        const char* peek() const { return begin() + readerIndex_; }

        /* 取特定长度数据 */
        void retrieve(size_t len) {
            assert(len <= readableBytes());
            readerIndex_ += len;
        }

        /* 取所有数据时，readerIndex和writerIndex都指向预留位置处 */
        void retrieveAll() {
            readerIndex_ = kCheapPrepend;
            writerIndex_ = kCheapPrepend;
        }

        /* 以string格式取出所有可读数据 */
        std::string retrieveAsString() {
            std::string str(peek(), readableBytes());
            retrieveAll();
            return str;
        }

        /* 添加str数据到buffer */
        void append(const std::string &str) {
            append(str.data(), str.length());
        }

        void append(const char* data, size_t len) {
            // 保证buffer可写入len大小数据，
            // 若不够，要扩容。
            ensureWritableBytes(len);
            // 拷贝数据到可写索引地址之后
            std::copy(data, data + len, beginWrite());
            //增加已写入数据大小
            hasWritten(len);
        }

        /* 确保Buffer可写入len数据 */
        void ensureWritableBytes(size_t len) {
            if (writableBytes() < len) {
                //若可写区域小于要写入大小，
                //则扩容
                makeSpace(len);
            }
            assert(writableBytes() >= len);
        }

        /* 返回可写数据索引首地址 */
        char* beginWrite() {
            return begin() + writerIndex_;
        }

        /* 返回可写数据首地址，外界不可修改返回的地址
           方法内部不可修改数据成员的值 */
        const char* beginWrite() const {
            return begin() + writerIndex_;
        }

        /* 修改已写数据大小 */
        void hasWritten(size_t len) {
            writerIndex_ += len;
        }

        /* 添加预分配数据 */
        void prepend(const void* data, size_t len) {
            // 预分配空间要大于要添加的数据
            assert(len <= prependableBytes());
            readerIndex_ -= len; // 将读索引前移
            const char* d = static_cast<const char *>(data);
            // 这里readerIndex_已经前移，拷贝数据
            std::copy(d, d + len, begin() + readerIndex_);
        }

        /* 从可读地址到可写地址区间，查找CRLF，beginWrite()是可写数据首地址，
         * 如果查找到的CRLF和beginWrite()地址一样，表示整个可读数据没有CRLF，
         * 若不是，则返回找到的CRLF的地址。 */
        const char* findCRLF() const {
            const char *crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
            return crlf == beginWrite() ? nullptr : crlf;
        }

        /* 指定起始地址~可写地址区间，查找CRLF */
        const char* findCRLF(const char *start) const {
            if (peek() > start) {
                return nullptr;
            }
            if (start > beginWrite()) {
                return nullptr;
            }
            const char *crlf = std::search(start, beginWrite(), kCRLF, kCRLF + 2);
            return crlf == beginWrite() ? nullptr : crlf;
        }

    private:
        char* begin() {
            return &*buffer_.begin();
        }

        const char* begin() const {
            return &*buffer_.begin();
        }

        void makeSpace(size_t len) {
            if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
                //可写空间不满足时，要重新分配内存，
                //这里用到了std::vector::resize，会对冲洗分配的内存初始化
                // FIXME: move readable data
                buffer_.resize(writerIndex_ + len);
            } else {
                //当可写空间满足要写入数据的长度时，
                //1、将可读数据往前拷贝（拷贝的首地址就是相对内存起始地址偏移kCheapPrepend）.
                //2、重新指定readerIndex和writerIndex的值。
                // move readable data to the front, make space inside buffer
                assert(kCheapPrepend < readerIndex_);
                size_t readable = readableBytes();
                std::copy(begin() + readerIndex_,
                          begin() + writerIndex_,
                          begin() + kCheapPrepend);
                readerIndex_ = kCheapPrepend;
                writerIndex_ = readerIndex_ + readable;
                assert(readable == readableBytes());
            }
        }

    private:
        std::vector<char> buffer_;  // Buffer实体
        size_t readerIndex_;        // 可读索引
        size_t writerIndex_;        // 可写索引

        static const char kCRLF[];  // 字符串结束标识“\r\n”
    };
}




#endif //KVDB_BUFFER_H
