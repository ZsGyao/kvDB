/**
  ******************************************************************************
  * @file           : Server_Start.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-4-2
  ******************************************************************************
  */


#include "DBServer.h"

int main() {
    kvDB::EventLoop loop;
    kvDB::InetAddress localAddr(9981);
    kvDB::DBServer dbServer(&loop,localAddr);

    dbServer.start();
    loop.loop();

    return 0;

}