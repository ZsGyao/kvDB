/**
  ******************************************************************************
  * @file           : DBClient_Start.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-4-1
  ******************************************************************************
  */

#include "DBClient.h"

int main() {
    kvDB::DBClient client(9981);

    client.Connect();
    client.parseCmd();

    return 0;
}