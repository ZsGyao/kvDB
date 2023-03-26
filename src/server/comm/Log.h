/**
  ******************************************************************************
  * @file           : Log.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-26
  ******************************************************************************
  */


#ifndef KVDB_LOG_H
#define KVDB_LOG_H

#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <semaphore.h>

namespace kvDB {

    /**
    * @brief 日志级别枚举类
    */
    enum LogLevel {
        DEBUG = 1,   // 调试 信息
        INFO  = 2,   // 一般 信息
        WARN  = 3,   // 警告 信息
        ERROR = 4,   // 错误 信息
        FATAL  = 5   // 致命 错误
    };

    /**
     * @brief 将 level 的字符串转为 Level
     * @param[in] str level 字符串
     * @return 日志等级枚举
     */
    LogLevel stringToLevel(const std::string& str);

    /**
     * @brief 将 LogLevel 转为 string
     * @param[in] level 日志等级
     * @return string 日志等级
     */
    std::string levelToString(const LogLevel& level);

    /**
     * @brief 是否打开日志
     * @return
     */
    bool OpenLog();

    /**
     * @brief 日志事件类
     */
    class LogEvent {
    public:
        typedef std::shared_ptr<LogEvent> ptr;

        /**
         * @brief 构造函数
         * @param level 日志等级
         * @param file_name 文件名
         * @param line 行号
         * @param func_name 执行的函数名
         * @param type 日志类型
         */
        LogEvent(LogLevel level, const char* file_name, int line, const char* func_name);

        /**
         * @brief 析构函数
         */
        ~LogEvent();

        /**
         * @brief 获取 stream 流
         * @return stream 流
         */
        std::stringstream& getStringStream();

        /**
         * @brief 在包装类析构时调用写入日志
         */
        void log();

    private:
        timeval           m_timeval;     // 时间结构体，日志发生时间
        LogLevel          m_level;       // 日志等级
        pid_t             m_pid = 0;     // 进程 id
        pid_t             m_tid = 0;     // 线程 id
        uint64_t          m_cor_id = 0;  // 协程 id
        const char*       m_file_name;   // 文件名
        int               m_line = 0;    // 行号
        const char*       m_func_name;   // 执行的函数名
        std::stringstream m_ss;          // stream 流
    };

    /**
     * @brief 日志包装类，在析构时写入日志
     */
    class LogWarp {
    public:
        explicit LogWarp(LogEvent::ptr event);
        ~LogWarp();
        std::stringstream& getStringStream();
    private:
        LogEvent::ptr m_event;
    };


    class Log {

    };
}

#endif //KVDB_LOG_H
