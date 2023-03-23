/**
  ******************************************************************************
  * @file           : InetAddress.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#ifndef KVDB_INETADDRESS_H
#define KVDB_INETADDRESS_H

#include <cstdint>
#include <string>
#include <netinet/in.h>

namespace kvDB {

    class InetAddress {
    public:
        explicit InetAddress(uint16_t port = 12345, const std::string& ip = "127.0.0.1");
        explicit InetAddress(const sockaddr_in& addr) : addr_(addr) {}

        std::string toIp() const;
        std::string toIpPort() const;
        uint16_t toPort() const;

        const sockaddr_in &getSockAddr() const { return addr_; }
        void setSockAddr(const sockaddr_in &addr) { addr_ = addr; }

    private:
        sockaddr_in addr_;
    };

}


#endif //KVDB_INETADDRESS_H
