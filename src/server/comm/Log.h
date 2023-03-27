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
#include <mutex>

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

    /**
     * @brief 异步写日志器
     */
    class AsyncLogger {
    public:
        typedef std::shared_ptr<AsyncLogger> ptr;

        /**
         * @brief 构造函数
         * @param file_name 写入文件名
         * @param file_path 写入文件路径
         * @param max_size 最大大小
         * @param type 日志类型
         */
        AsyncLogger(std::string file_name, std::string file_path, int max_size);

        /**
         * @brief 析构函数
         */
        ~AsyncLogger();

        /**
         * @brief 向任务队列中加入任务
         * @param buffer 待写入的日志集合
         */
        void push(std::vector<std::string>& buffer);

        /**
         * @brief 将缓冲区的数据刷写到文件
         */
        void flush();

        /**
         * @brief 执行异步写入日志
         * @param arg 要写入的参数
         * @attention 当条件m_condition满足被唤醒时，就开始写入硬盘
         */
        static void* execute(void* arg);

        /**
         * @brief 停止异步写日志
         */
        void stop();

    public:
        // 日志写入的任务队列，vector保存要写入的日志
        std::queue<std::vector<std::string>> m_tasks;

    private:
        std::string    m_file_name;                // 写入文件名
        std::string    m_file_path;                // 日志写入路径
        int            m_max_size = 0;             // 写入单个文件最大大小
        int            m_no = 0;                   // 写入文件的下标
        bool           m_need_reopen = false;      // 是否需要重新打开文件
        FILE*          m_file_handle = nullptr;    // 文件句柄
        std::string    m_date;                     // 日期

        std::mutex     m_mutex;                    // 互斥量
        pthread_cond_t m_condition;                // 线程初始化条件
        bool           m_stop = false;             // 异步写入是否停止

    public:
        pthread_t m_thread;
        sem_t     m_semaphore;
    };

    class Logger {
    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger();

        ~Logger();

        void init(const std::string& file_name, const std::string& file_path, int max_size, int sync_interval);

        // void log();
        void pushLog(const std::string& log_msg);

        /**
         * @brief 取出保存在日志器中的任务，以定时任务的形式，加入异步日志器，去写入
         */
        void loopFunc();

        /**
         * @brief 停止并强制写入所有的日志
         */
        void flush();

        /**
         * @brief 启动日志器
         */
        void start();

        AsyncLogger::ptr getAsyncLogger() {
            return m_async_logger;
        }

    public:
        std::vector<std::string> m_log_buffer;     // 保存RPC日志

    private:
        std::mutex       m_log_buff_mutex;
        bool             m_is_init = false;    // 是否初始化
        AsyncLogger::ptr m_async_logger;       // 异步写入日志的AsyncLogger实例指针

        int              m_sync_interval = 0;
    };

}

#endif //KVDB_LOG_H
