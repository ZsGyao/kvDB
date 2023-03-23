/**
  ******************************************************************************
  * @file           : Timestamp.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-23
  ******************************************************************************
  */


#ifndef KVDB_TIMESTAMP_H
#define KVDB_TIMESTAMP_H

#include <string>

namespace kvDB {

    class Timestamp {
    public:
        Timestamp() = default;

        explicit Timestamp(int64_t microSecondsSinceEpoch);

        std::string toString(bool showMicroSeconds = false) const;

        int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }

        bool valid() const{ return microSecondsSinceEpoch_ > 0;}

        void swap(Timestamp & that){
            std::swap(microSecondsSinceEpoch_,that.microSecondsSinceEpoch_);
        }

    public:
        static Timestamp now();
        static Timestamp invalid();

    public:
        static const int kMicroSecondsPerSecond = 1000 * 1000;
        static const int kMilliSecondsPerSecond = 1000;
        static const int kMicroSecondsPerMilliSecond = 1000;

    private:
        int64_t microSecondsSinceEpoch_ = 0;
    };

    inline bool operator<(Timestamp lhs,Timestamp rhs){
        return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
    }
    inline bool operator<=(Timestamp lhs,Timestamp rhs){
        return lhs.microSecondsSinceEpoch() <= rhs.microSecondsSinceEpoch();
    }
    inline bool operator>(Timestamp lhs,Timestamp rhs){
        return lhs.microSecondsSinceEpoch() > rhs.microSecondsSinceEpoch();
    }
    inline bool operator>=(Timestamp lhs,Timestamp rhs){
        return lhs.microSecondsSinceEpoch() >= rhs.microSecondsSinceEpoch();
    }
    inline bool operator==(Timestamp lhs,Timestamp rhs){
        return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
    }
    inline Timestamp operator+(Timestamp lhs,Timestamp rhs){
        return Timestamp(lhs.microSecondsSinceEpoch() + rhs.microSecondsSinceEpoch());
    }
    inline Timestamp operator-(Timestamp lhs,Timestamp rhs){
        return Timestamp(lhs.microSecondsSinceEpoch() - rhs.microSecondsSinceEpoch());
    }
    inline Timestamp addTime(Timestamp timestamp,double seconds){
        int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
        return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
    }

}



#endif //KVDB_TIMESTAMP_H
