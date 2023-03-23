/**
  ******************************************************************************
  * @file           : Singleton.h
  * @author         : zgys
  * @brief          : 单例模板类
  * @attention      : C++11的智能指针和自解锁等，巧妙避免了所有问题，并实现自动GC。
  * @date           : 23-3-23
  ******************************************************************************
  */


#ifndef KVDB_SINGLETON_H
#define KVDB_SINGLETON_H

#include <memory>
#include <mutex>

namespace kvDB {
    template<typename T>
    class Singleton {
    public:
        //获取全局单例对象
        template<typename ...Args>
        static std::shared_ptr<T> GetInstance(Args&&... args) {
            if (!m_pSingleton) {
                std::lock_guard<std::mutex> gLock(m_Mutex);
                if (nullptr == m_pSingleton) {
                    m_pSingleton = std::make_shared<T>(std::forward<Args>(args)...);
                }
            }
            return m_pSingleton;
        }

        //主动析构单例对象（一般不需要主动析构，除非特殊需求）
        static void DesInstance() {
            if (m_pSingleton) {
                m_pSingleton.reset();
                m_pSingleton = nullptr;
            }
        }

    private:
        explicit Singleton();
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
        ~Singleton();

    private:
        static std::shared_ptr<T> m_pSingleton;
        static std::mutex m_Mutex;
    };

    template<typename T>
    std::shared_ptr<T> Singleton<T>::m_pSingleton = nullptr;

    template<typename T>
    std::mutex Singleton<T>::m_Mutex;
}

#endif //KVDB_SINGLETON_H
