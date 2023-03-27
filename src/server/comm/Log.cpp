/**
  ******************************************************************************
  * @file           : Log.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-26
  ******************************************************************************
  */


#include "Log.h"
#include <atomic>
#include <unistd.h>
#include <stdio.h>
#include <ctime>
#include <sys/syscall.h>
#include <sys/time.h>
#include <assert.h>
#include <signal.h>
#include <iostream>
#include <algorithm>

namespace kvDB {
    /// LOG日志写入的index
    static std::atomic<int64_t> g_log_index{0};

    extern kvDB::Logger::ptr gLogger;

    void CoredumpHandler(int signal_no) {
        // ErrorLog << "progress received invalid signal, will exit";
        printf("progress received invalid signal, will exit\n");
        gLogger->flush();
        pthread_join(gLogger->getAsyncLogger()->m_thread, nullptr);

        signal(signal_no, SIG_DFL);
        raise(signal_no);
    }

    static thread_local pid_t t_thread_id = 0;   // 线程号
    static pid_t g_pid = 0;                      // 进程号

    /* 获取线程id */
    pid_t gettid() {
        if (t_thread_id == 0) {
            t_thread_id = syscall(SYS_gettid);
        }
        return t_thread_id;
    }

    LogLevel stringToLevel(const std::string& str) {
        if (str == "DEBUG")
            return LogLevel::DEBUG;

        if (str == "INFO")
            return LogLevel::INFO;

        if (str == "WARN")
            return LogLevel::WARN;

        if (str == "ERROR")
            return LogLevel::ERROR;

        if (str == "FATAL")
            return LogLevel::FATAL;

        return LogLevel::DEBUG;
    }

    std::string levelToString(const LogLevel level) {
        std::string le = "DEBUG";
        switch (level) {
            case DEBUG:
                le = "DEBUG";
                return le;
            case INFO:
                le = "INFO";
                return le;
            case WARN:
                le = "WARN";
                return le;
            case ERROR:
                le = "ERROR";
                return le;
            case FATAL:
                le = "FATAL";
                return le;
            default:
                return le;
        }
    }

    bool OpenLog() {
        if (!gLogger) {
            return false;
        }
        return true;
    }

    LogEvent::LogEvent(LogLevel level, const char *file_name, int line, const char *func_name)
            : m_level(level),
              m_file_name(file_name),
              m_line(line),
              m_func_name(func_name) {
    }

    LogEvent::~LogEvent() {
    }

    std::stringstream& LogEvent::getStringStream() {
        gettimeofday(&m_timeval, nullptr);
        struct tm time;  // ISO C `broken-down time' structure.
        /* Return the `struct tm' representation of *TIMER in local time,
          using *TP to store the result.  */
        localtime_r(&(m_timeval.tv_sec), &time);
        const char *format = "%Y-%m-%d %H:%M:%S";
        char buf[128];
        strftime(buf, sizeof(buf), format, &time);
        m_ss << "[" << buf << "]\t";

        std::string s_level = kvDB::levelToString(m_level);
        m_ss << "[" << s_level << "]\t";

        if (g_pid == 0) {
            g_pid = getpid();
        }
        m_pid = g_pid;

        if (t_thread_id == 0) {
            t_thread_id = gettid();
        }
        m_tid = t_thread_id;

        m_ss << "[" << m_pid << "]\t"
             << "[" << m_tid << "]\t"
             << "[" << m_file_name << ":" << m_line << "]\t";

        return m_ss;
    }

    /**
     * 比如写入一个 DebugLog << "Hello zRPC Log";
     * 1. 调用宏，LogWarp包装一个 log_event，在运行完这一行，到下一行时，LogWarp析构，调用 log_event->log()
     * 2. 根据 log_event 类型将 流保存到不同的容器中
     */
    void LogEvent::log() {
        m_ss << "\n";
        if (m_level >= kvDB::gConfig->m_log_level) {
            gLogger->pushLog(m_ss.str());                           // 把写入的
        }
    }

    LogWarp::LogWarp(LogEvent::ptr event) : m_event(std::move(event)) {
    }

    LogWarp::~LogWarp() {
        m_event->log();
    }

    std::stringstream &LogWarp::getStringStream() {
        return m_event->getStringStream();
    }

    AsyncLogger::AsyncLogger(std::string file_name, std::string file_path, int max_size)
            : m_file_name(file_name),
              m_file_path(file_path),
              m_max_size(max_size) {

        int rt = sem_init(&m_semaphore, 0, 0);
        assert(rt == 0);

        //  获取当前时间并格式化
        timeval now;
        gettimeofday(&now, nullptr);
        struct tm now_time;
        localtime_r(&(now.tv_sec), &now_time);

        const char *format = "%Y-%m-%d";    // 2023-03-27
        char date[32];
        strftime(date, sizeof(date), format, &now_time);
        m_date = std::string(date);

        /* Create a new thread, starting with execution of START-ROUTINE
           getting passed ARG.  Creation attributed come from ATTR.  The new
           handle is stored in *NEWTHREAD.  */
        rt = pthread_create(&m_thread, nullptr, &AsyncLogger::execute, this);
        assert(rt == 0);
        rt = sem_wait(&m_semaphore);
        assert(rt == 0);
    }

    AsyncLogger::~AsyncLogger() {
    }

    void AsyncLogger::push(std::vector<std::string>& buffer) {
        if (!buffer.empty()) {
            MutexType::Lock lock(m_mutex);
            m_tasks.push(buffer);
            lock.unlock();
            // 唤醒信号
            pthread_cond_signal(&m_condition);
        }
    }

    void AsyncLogger::flush() {
        if (m_file_handle) {
            fflush(m_file_handle);
        }
    }

    /* 当条件m_condition满足被唤醒时，就开始写入硬盘 */
    void* AsyncLogger::execute(void *arg) {

//        std::cout << "**** Debug [log.cpp " << __LINE__ << "], AsyncLogger::execute [threadId:" << gettid() << "] *****"
//                  << std::endl;

        // 获取当前AsyncLogger实例
        AsyncLogger *ptr = reinterpret_cast<AsyncLogger *>(arg);
        // std::cout << "Debug [log.cpp "<< __LINE__ <<"], get AsyncLogger* ptr [threadId:" << gettid() << "]" << std::endl;
        /* Initialize condition variable COND using attributes ATTR, or use
           the default values if later is NULL.  */
        int rt = pthread_cond_init(&ptr->m_condition, nullptr);
        assert(rt == 0);

        // 信号量加一
        rt = sem_post(&ptr->m_semaphore);
        assert(rt == 0);
        // std::cout << "Debug [log.cpp "<< __LINE__ <<"], before into execute loop [threadId:" << gettid() << "]" << std::endl;
        while (true) {
            MutexType::Lock lock(ptr->m_mutex);

            while (ptr->m_tasks.empty() && !ptr->m_stop) { // 如果任务队列为空 并且写入未停止，线程条件等待
//                std::cout << "Debug [log.cpp " << __LINE__ << "] into execute loop, pthread_cond_wait [threadId:"
//                          << gettid() << "]" << std::endl;
                pthread_cond_wait(&(ptr->m_condition), ptr->m_mutex.getMutex());
            }

//            std::cout << "Debug [log.cpp " << __LINE__
//                      << "] into execute loop, pthread_cond_signal, try to get task [threadId:" << gettid() << "]"
//                      << std::endl;

            // 取出任务队列中的第一个任务
            std::vector<std::string> tmp;
            tmp.swap(ptr->m_tasks.front());
            ptr->m_tasks.pop();
            bool is_stop = ptr->m_stop;
            lock.unlock();

//            std::cout << "Debug [log.cpp " << __LINE__
//                      << "] in execute loop, already put into std::vector<std::string> tmp [tmp.size:"
//                      << tmp.size() << "] [threadId:" << gettid() << "]" << std::endl;

            // 获取当前时间并格式化
            timeval now;
            gettimeofday(&now, nullptr);
            struct tm now_time;
            localtime_r(&(now.tv_sec), &now_time);

            const char* format = "%Y-%m-%d";    // 2023-03-10
            char date[32];
            strftime(date, sizeof(date), format, &now_time);

            if (ptr->m_date != std::string(date)) { // 说明过了一天，重置文件名
                // cross day
                // reset m_no m_date
                ptr->m_no = 0;
                ptr->m_date = std::string(date);
                ptr->m_need_reopen = true;
            }

            if (!ptr->m_file_handle) { // 如果文件句柄不存在
                ptr->m_need_reopen = true;
            }

            std::stringstream ss;
            ss << ptr->m_file_path << "/" << ptr->m_file_name << "_" << ptr->m_date << "_"
               << LogTypeToString(ptr->m_log_type) << "_" << ptr->m_no << ".log";
            std::string full_file_name = ss.str();  // 从流中生成日志文件名

//            std::cout << "Debug [log.cpp " << __LINE__ << "] in execute loop, full_file_name create [" << full_file_name
//                      << "]" << std::endl;
//
//            std::cout << "Debug [log.cpp " << __LINE__ << "] m_need_reopen:" << ptr->m_need_reopen
//                      << " LogType:"<< ptr->m_log_type << " threadId:" << gettid() << std::endl;

            if (ptr->m_need_reopen) { // 如果需要重新打开
                if (ptr->m_file_handle) { // 如果打开过，文件句柄存在
                    fclose(ptr->m_file_handle); // 关闭
                }
                /* 以附加的方式打开只写文件。若文件不存在，则会创建该文件；
                   如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留（EOF 符保留）。 */
                ptr->m_file_handle = fopen(full_file_name.c_str(), "a"); // 打开文件
                if(!ptr->m_file_handle) {
                    std::cout << "fopen error! " << strerror(errno) << std::endl;
                    Exit(0);
                }
                ptr->m_need_reopen = false;
            }
            //  ftell 用于得到文件位置指针当前位置相对于文件首的偏移字节数
//            std::cout<< "------ ptr->m_file_handle size: XXXXXXX" << " --------" << std::endl;
            // ToDo ptr指针的m_file_handle为空，改
            auto size = ftell(ptr->m_file_handle);
            if(size < 0){
                std::cout << "ftell() error" << std::endl;
            }
//            std::cout<< "------ ptr->m_file_handle size:" << size << " --------" << std::endl;
            if ( size > ptr->m_max_size) { // 如果写日志文件超出容量
                fclose(ptr->m_file_handle);                    // 关闭此文件

                // single log file over max size
                ptr->m_no++;                                         // 写入日志文件名角标加一
                std::stringstream ss2;
                ss2 << ptr->m_file_path << "/" << ptr->m_file_name << "_" << ptr->m_date << "_"
                    << LogTypeToString(ptr->m_log_type) << "_" << ptr->m_no << ".log";
                full_file_name = ss2.str();   // 从流中生成日志文件名

//                std::cout << "Debug [log.cpp " << __LINE__ << "] in execute loop, full_file_name(out of day) recreate ["
//                          << full_file_name << "]" << std::endl;

                ptr->m_file_handle = fopen(full_file_name.c_str(), "a");
                ptr->m_need_reopen = false;
            }

            if (!ptr->m_file_handle) {
                std::cout << "open log file " << full_file_name.c_str() << "error!" << std::endl;
            }

//            std::cout << "Debug [log.cpp " << __LINE__ << "] in execute loop, try to fwrite [m_file_handle:"
//                      << ptr->m_file_handle << "]" << std::endl;

            // 开始写入，从vector中取出要写入的日志string
            for (const auto& i: tmp) {
                if (!i.empty()) {
                    fwrite(i.c_str(), 1, i.length(), ptr->m_file_handle);
//                    std::cout << "Debug [log.cpp " << __LINE__ << "], fwrite {log string:" << i << std::endl;
                }
            }
            tmp.clear();
            fflush(ptr->m_file_handle);
            if (is_stop) {
                break;
            }
        }
        if (ptr->m_file_handle) {
            fclose(ptr->m_file_handle);
            ptr->m_file_handle = nullptr;
        }

        return nullptr;
    }

}