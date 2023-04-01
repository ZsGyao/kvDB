/**
  ******************************************************************************
  * @file           : DBClient.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-4-1
  ******************************************************************************
  */


#ifndef KVDB_DBCLIENT_H
#define KVDB_DBCLIENT_H

#include <string>

namespace kvDB {
    class DBClient {
    public:
        DBClient(int port, std::string ip = "127.0.0.1");

        ~DBClient();

        void Connect();

        void Close() const;

        void Send(const std::string &buf) const;

        void Recv(char *buf) const ;

        void parseCmd();

        void handleRequest(const std::string &buf);

    private:
        std::string ip_;
        int         port_;
        int         connfd_;
    };

    const std::string helpTxt = "String: set, get\r\n"
                                "List: rpush, rpop\r\n"
                                "Hash: hset, hget, hgetall\r\n"
                                "Set: sadd, smembers\r\n"
                                "HSet: zadd, zcard, zrange, zcount, zgetall\r\n";

}


#endif //KVDB_DBCLIENT_H
