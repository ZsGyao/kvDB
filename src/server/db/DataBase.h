/**
  ******************************************************************************
  * @file           : DataBase.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-4-1
  ******************************************************************************
  */


#ifndef KVDB_DATABASE_H
#define KVDB_DATABASE_H

#include <ext/pool_allocator.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <list>
#include <map>
#include <unordered_set>
#include "../comm/Timestamp.h"
#include "SkipList.h"

namespace kvDB {

        typedef std::shared_ptr<SkipList> SP_SkipList;

        template<typename T1, typename T2>
        using Dict = std::unordered_map<T1, T2, std::hash<T1>,
                                        std::equal_to<T1>,
                                        __gnu_cxx::__pool_alloc<std::pair<const T1, T2>>>;


        typedef Dict<std::string, std::string> String;
        typedef Dict<std::string, std::list<std::string, __gnu_cxx::__pool_alloc<std::string>>> List;
        typedef Dict<std::string, std::map<std::string, std::string, std::less<>, __gnu_cxx::__pool_alloc<std::pair<const std::string, std::string>>>> Hash;
        typedef Dict<std::string, std::unordered_set<std::string, std::hash<std::string>, std::equal_to<>, __gnu_cxx::__pool_alloc<std::string>>> Set;
        typedef Dict<std::string, SP_SkipList> ZSet;

        typedef Dict<std::string, Timestamp> Expire;

        class Database {
        public:
            Database() = default;

            ~Database() = default;

            /* 导入持久化的rdb文件 */
            void rdbLoad(int index);

            /* 添加K-V */
            bool addKey(const int type, const std::string& key, const std::string& objKey,
                        const std::string& objValue);

            /* 删除K-V */
            bool delKey(const int type, const std::string& key);

            /* 查找K-V 如果查找ZSet，使用 key:low@high 可以查找范围内的K-V， 如果key设置过期并且已经过期，
             * lazy delete，在get的时候删除*/
            std::string getKey(const int type, const std::string& key);

            /* 设置过期时间，入参expiredTime： expiredTime毫秒后过期 */
            bool setPExpireTime(const int type, const std::string& key, double expiredTime);

            /* 设置过期时间，入参expiredTime： 时间戳为expiredTime时过期 */
            bool setPExpireTime(const int type, const std::string& key, const Timestamp& expiredTime);

            /* 获取key的过期时间，未设置过期时间，返回 Timestamp::invalid() */
            Timestamp getKeyExpiredTime(const int type, const std::string& key);

            /* 判断key是否过期 */
            bool judgeKeyExpiredTime(const int type, const std::string& key);

            const std::string rpopList(const std::string& key);

        public:
            String& getKeyStringObj() {
                return String_;
            }

            List& getKeyListObj() {
                return List_;
            }

            Hash& getKeyHashObj() {
                return Hash_;
            }

            Set& getKeySetObj() {
                return Set_;
            }

            ZSet& getKeyZSetObj(){
                return ZSet_;
            }

            // 得到当前数据库键的数目
            int getKeySize() const {
                return getKeyStringSize() + getKeyListSize() + getKeyHashSize() + getKeySetSize() + getKeyZSetSize();
            }

            int getKeyStringSize() const {
                return String_.size();
            };

            int getKeyListSize() const {
                return List_.size();
            }

            int getKeyHashSize() const {
                return Hash_.size();
            }

            int getKeySetSize() const {
                return Set_.size();
            }

            int getKeyZSetSize() const {
                return ZSet_.size();
            }

        private:
            /* 截取p1-p2的字符串*/
            std::string interceptString(const std::string& ss, int p1, int p2);

            String String_;           // 保存type为dbString类型的K-V
            List   List_;             // 保存type为dbList类型的K-V
            Hash   Hash_;             // 保存type为dbHash类型的K-V
            Set    Set_;              // 保存type为dbSet类型的K-V
            ZSet   ZSet_;             // 保存type为dbZSet类型的K-V

            Expire StringExpire_;     // 保存dbString的过期时间，first->key , second->expire_time
            Expire ListExpire_;
            Expire HashExpire_;
            Expire SetExpire_;
            Expire ZSetExpire_;
    };
}


#endif //KVDB_DATABASE_H
