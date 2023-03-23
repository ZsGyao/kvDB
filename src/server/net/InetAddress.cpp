/**
  ******************************************************************************
  * @file           : InetAddress.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#include "InetAddress.h"
#include <cstring>
#include <arpa/inet.h>
#include "InetAddress.h"

namespace kvDB {

    InetAddress::InetAddress(uint16_t port, const std::string& ip) {
        bzero(&addr_,sizeof addr_);

        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    }

    std::string InetAddress::toIp() const{
        char buf[64] = {0};
        /* 将IPv4或IPv6 Internet网络地址转换为 Internet标准格式的字符串 */
        ::inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof buf);
        return buf;
    }

    std::string InetAddress::toIpPort() const{
        char buf[64] = {0};
        ::inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof buf);

        size_t end = strlen(buf);
        uint16_t port = ntohs(addr_.sin_port);
        sprintf(buf + end,":%u",port);
        return buf;
    }

    uint16_t InetAddress::toPort() const{
        return ntohs(addr_.sin_port);
    }

}