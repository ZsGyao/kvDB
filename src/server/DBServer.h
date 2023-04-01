/**
  ******************************************************************************
  * @file           : DBServer.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-4-1
  ******************************************************************************
  */


#ifndef KVDB_DBSERVER_H
#define KVDB_DBSERVER_H

#include <vector>
#include <string>
#include "./net/EventLoop.h"
#include "./net/InetAddress.h"
#include "./net/TcpConnection.h"
#include "./net/Buffer.h"
#include "./net/Server.h"
#include "./db/DataBase.h"

namespace kvDB {
    class DBServer {
    public:
        typedef std::vector<std::string> VctS;

        /* 设置服务的连接回调，消息到达回调，初始化数据库 */
        DBServer(EventLoop* loop, const InetAddress& localAddr);

        ~DBServer() = default;

        void onConnection(const TcpConnectionPtr&);

        /* 当消息到来时，执行的回调，取出应用层缓存中的消息，解析命令返回结果 */
        void onMessage(const TcpConnectionPtr&,
                       Buffer* buf,
                       Timestamp);

        /* 启动网络服务 */
        void start();

        /*进行rdb持久化*/
        void rdbSave();

    private:
        // 数据分库的数目
        static const long DEFAULT_DB_NUM = 16;

        /* 初始化数据库，绑定数据库命令 */
        void initDB();

        /* 解析命令，将传入的参数保存到命令字典中对应命令的Vecs */
        std::string parseMsg(const std::string& msg);

        std::string setCommand(VctS &&);

        std::string getCommand(VctS &&);

        std::string pExpiredCommand(VctS &&);

        std::string expiredCommand(VctS &&);

        std::string bgsaveCommand(VctS &&);

        std::string selectCommand(VctS &&);

        std::string rpushCommand(VctS &&);

        std::string rpopCommand(VctS &&);

        std::string hsetCommand(VctS &&);

        std::string hgetCommand(VctS &&);

        std::string hgetAllCommand(VctS &&);

        std::string saddCommand(VctS &&);

        std::string smembersCommand(VctS &&);

        std::string zaddCommand(VctS &&);

        std::string zcardCommand(VctS &&);

        std::string zrangeCommand(VctS &&);

        std::string zcountCommand(VctS &&);

        std::string zgetAllCommand(VctS &&);

        std::string saveHead();

        std::string saveSelectDB(const int index);

        std::string saveExpiredTime(const Timestamp expiredTime);

        std::string saveType(const int type);

        std::string saveKV(const std::string& key, const std::string& value);

        bool checkSaveCondition();

        // db相关
        std::vector<std::unique_ptr<Database>> database_; // 分库管理Database的容器
        int dbIndex;                                      // 数据库的index
        /* 保存所有命令应该调用的接口  first-->cmd  second-->cmd对应的处理函数，Vcts保存parseMsg()解析的传入 */
        std::unordered_map<std::string, std::function<std::string(VctS &&)>> cmdDict;
        Timestamp lastSave_;     // 最后一次进行RDB落盘

        // net相关
        EventLoop* loop_;
        Server server_;
    };
}

#endif //KVDB_DBSERVER_H
