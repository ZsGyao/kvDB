/**
  ******************************************************************************
  * @file           : DBStatus.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-4-1
  ******************************************************************************
  */


#ifndef KVDB_DBSTATUS_H
#define KVDB_DBSTATUS_H

#include <string>

namespace kvDB {
    class DBStatus {
    public:
        DBStatus() : dbState_(resCode::kOK), msg_("") {}

        DBStatus(int dbState, const std::string msg)
                : dbState_(dbState),
                  msg_(msg) {
        }

        ~DBStatus() = default;

        static DBStatus Ok(){
            return DBStatus();
        }

        static DBStatus notFound(const std::string & msg){
            return DBStatus(kNotFound, msg);
        }

        static DBStatus IOError(const std::string & msg){
            return DBStatus(kIOError, msg);
        }

        std::string toString(){
            if(msg_ == ""){
                return "OK";
            } else {
                const char* type;
                switch (dbState_) {
                    case kOK:
                        type = "OK";
                        break;
                    case kNotFound:
                        type = "NotFound: ";
                        break;
                    case kIOError:
                        type = "IO Error: ";
                        break;
                    default:
                        break;
                }
                std::string res(type);
                res.append(msg_);
                return res;
            }
        }

    private:
        enum resCode {
            kOK = 0,
            kNotFound,
            kIOError
        };

        int dbState_;
        std::string msg_;
    };

}

#endif //KVDB_DBSTATUS_H
