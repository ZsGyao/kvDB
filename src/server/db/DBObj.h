/**
  ******************************************************************************
  * @file           : DBObj.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-4-1
  ******************************************************************************
  */


#ifndef KVDB_DBOBJ_H
#define KVDB_DBOBJ_H

#include <string>
#include "../comm/Timestamp.h"

namespace kvDB {
    // 数据类型
    const short dbString = 0;
    const short dbList   = 1;
    const short dbHash   = 2;
    const short dbSet    = 3;
    const short dbZSet   = 4;


    const std::string defaultObjValue = "NULL";

    //RDB默认保存时间(ms)
    const Timestamp rdbDefaultTime(1000 * Timestamp::kMicroSecondsPerMilliSecond);
}

#endif //KVDB_DBOBJ_H
