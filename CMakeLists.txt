cmake_minimum_required(VERSION 3.22)
project(kvDB)

set(CMAKE_C_STANDARD 11)

include (src/server/comm/cmake/utils.cmake)

# -rdynamic: 将所有符号都加入到符号表中，便于使用dlopen或者backtrace追踪到符号
# -fPIC: 生成位置无关的代码，便于动态链接
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -fPIC")

# 指定编译选项
#set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -std=c++11 -O0 -ggdb -Wall -Werror")
#set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -std=c++11 -O0 -ggdb -Wall")

# -Wno-unused-function: 不要警告未使用函数
# -Wno-builtin-macro-redefined: 不要警告内置宏重定义，用于重定义内置的__FILE__宏
# -Wno-deprecated: 不要警告过时的特性
# -Wno-deprecated-declarations: 不要警告使用带deprecated属性的变量，类型，函数
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated -Wno-deprecated-declarations")


include_directories(.)
include_directories(src)

option(BUILD_TEST "ON for complile test" ON)

set(LIB_SRC
        src/server/comm/Logger.cpp
        src/server/comm/Timestamp.cpp
        src/server/net/Buffer.cpp
        src/server/net/InetAddress.cpp
        src/server/net/Socket.cpp
        src/server/net/EventLoop.cpp src/server/net/EventLoop.h src/server/net/Channel.cpp src/server/net/Channel.h src/server/net/EpollPoller.cpp src/server/net/EpollPoller.h src/server/net/Acceptor.cpp src/server/net/Acceptor.h src/server/net/TcpConnection.cpp src/server/net/TcpConnection.h src/server/net/Callbacks.h src/server/comm/Log.cpp src/server/comm/Log.h src/server/net/Server.cpp src/server/net/Server.h)

set(LIBS
        src
        )

add_library(src SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(src)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)