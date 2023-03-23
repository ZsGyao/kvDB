/**
  ******************************************************************************
  * @file           : Logger.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#ifndef KVDB_LOGGER_H
#define KVDB_LOGGER_H

#include <string>
#include <memory>
#include "Singleton.h"
#include "Noncopyable.h"

namespace kvDB {

#define LOG_INFO(logmsg,...)                                            \
    do                                                                  \
    {                                                                   \
       Logger::ptr logger = kvDB::Singleton_Logger::GetInstance();      \
       logger->setLogLevel(INFO);                                        \
       char buf[1024] = {0};                                            \
       snprintf(buf,1024,logmsg, ##__VA_ARGS__);                        \
       logger->log(buf);                                                 \
    }while(0)

#ifdef MUDEBUG
    #define LOG_DEBUG(logmsg,...)                                      \
    do                                                                 \
    {                                                                  \
       Logger::ptr logger = kvDB::Singleton_Logger::GetInstance();     \
       logger->setLogLevel(DEBUG);                                      \
       char buf[1024] = {0};                                           \
       snprintf(buf,1024,logmsg, ##__VA_ARGS__);                       \
       logger->log(buf);                                                \
    }while(0)
#else
#define LOG_DEBUG(logmsg,...)
#endif

#define LOG_WARN(logmsg,...)                                          \
    do                                                                \
    {                                                                 \
       Logger::ptr logger = kvDB::Singleton_Logger::GetInstance();    \
       logger->setLogLevel(WARN);                                      \
       char buf[1024] = {0};                                          \
       snprintf(buf,1024,logmsg,##__VA_ARGS__);                       \
       logger->log(buf);                                               \
    }while(0)

#define LOG_ERROR(logmsg,...)                                         \
    do                                                                \
    {                                                                 \
       Logger::ptr logger = kvDB::Singleton_Logger::GetInstance();    \
       logger->setLogLevel(ERROR);                                     \
       char buf[1024] = {0};                                          \
       snprintf(buf,1024,logmsg,##__VA_ARGS__);                       \
       logger->log(buf);                                               \
    }while(0)

#define LOG_FATAL(logmsg,...)                                         \
    do                                                                \
    {                                                                 \
       Logger::ptr logger = kvDB::Singleton_Logger::GetInstance();    \
       logger->setLogLevel(FATAL);                                     \
       char buf[1024] = {0};                                          \
       snprintf(buf,1024,logmsg,##__VA_ARGS__);                       \
       logger->log(buf);                                               \
       exit(-1);                                                      \
    }while(0)


    enum LogLevel {
        DEBUG = 1,   // 调试 信息
        INFO  = 2,   // 一般 信息
        WARN  = 3,   // 警告 信息
        ERROR = 4,   // 错误 信息
        FATAL = 5    // 致命 错误
    };

    /* 日志类（单例) */
class Logger : public Noncopyable {
    public:
        typedef std::shared_ptr<Logger> ptr;

        // 设置日志级别
        void setLogLevel(int level);
        // 写日志
        void log(const std::string& msg) const;

    private:
        int logLevel_;
    };

typedef kvDB::Singleton<Logger> Singleton_Logger;

}


#endif //KVDB_LOGGER_H
