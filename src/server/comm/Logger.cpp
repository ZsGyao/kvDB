/**
  ******************************************************************************
  * @file           : Logger.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */

#include <iostream>
#include "Logger.h"
#include "Timestamp.h"

namespace kvDB {

    // 设置日志级别
    void Logger::setLogLevel(int level) {
        logLevel_ = level;
    }

    // 写日志 [级别] time:msg
    void Logger::log(const std::string& msg) const {
        switch (logLevel_) {
            case INFO:
                std::cout<<"[INFO]";
                break;
            case DEBUG:
                std::cout<<"[DEBUG]";
                break;
            case ERROR:
                std::cout<<"[ERROR]";
                break;
            case FATAL:
                std::cout<<"[FATAL]";
                break;
            default:
                break;
        }
        // 输出time和msg
        std::cout <<  Timestamp::now().toString() << " : " << msg << std::endl;
    }

}