/**
  ******************************************************************************
  * @file           : DBClient.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-4-1
  ******************************************************************************
  */


#include "DBClient.h"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

namespace kvDB {
    DBClient::DBClient(int port, std::string ip)
            :port_(port),
             ip_(std::move(ip)),
             connfd_(-1){

    }

    DBClient::~DBClient() = default;

    void DBClient::Connect() {
        struct sockaddr_in peerAddr;
        bzero(&peerAddr,sizeof(peerAddr));
        peerAddr.sin_family = AF_INET;
        peerAddr.sin_port = htons(port_);
        inet_pton(PF_INET,ip_.c_str(),&peerAddr.sin_addr);

        connfd_ = socket(AF_INET,SOCK_STREAM,0);
        while(true) {
            int ret = connect(connfd_, (struct sockaddr *) &peerAddr, sizeof(peerAddr));
            if(ret == 0){
                std::cout << "connection successful"<<std::endl;
                return ;
            }else if (ret == -1) {
                if(errno == EINTR){
                    std::cout << "connection interrupted by signal,try again..."<<std::endl;
                    continue;
                }else if(errno == EINPROGRESS) {
                    break;
                }else{
                    std::cout << "connection error"<<std::endl;
                    close(connfd_);
                    abort();
                }
            }
        }
    }

    void DBClient::Close() const {
        close(connfd_);
    }

    void DBClient::Send(const std::string &buf) const {
        ::write(connfd_,buf.c_str(),buf.size());
    }

    void DBClient::Recv(char* buf) const {
        ::read(connfd_,buf,1024);
    }

    void DBClient::parseCmd() {
        char buf[1024] = {0};

        std::cout << "kvDB> ";
        while(fgets(buf, sizeof(buf), stdin) != NULL)
        {
            std::string tmp(buf, buf + strlen(buf));
            int pos = tmp.find_last_of('\n');
            tmp = tmp.substr(0, pos);
            if(tmp == "quit")
                break;
            if(tmp == "help"){
                std::cout<<helpTxt;
                std::cout << "kvDB> ";
                continue;
            }
            handleRequest(tmp);
            std::cout << "kvDB> ";
        }
    }

    void DBClient::handleRequest(const std::string &buf) {
        char buffer[1024] = {0};

        Send(buf);
        Recv(buffer);
        std::cout << buffer << std::endl;
    }

}