/**
  ******************************************************************************
  * @file           : Noncopyable.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#ifndef KVDB_NONCOPYABLE_H
#define KVDB_NONCOPYABLE_H

namespace kvDB {
    /* 不可拷贝 赋值 */
    class Noncopyable{
    protected:
        Noncopyable() = default;
        ~Noncopyable() = default;

    public:
        Noncopyable(const Noncopyable&) = delete;
        Noncopyable& operator=(const Noncopyable&) = delete;
    };
}

#endif //KVDB_NONCOPYABLE_H
