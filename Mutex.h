#ifndef bs_Mutex_h
#define bs_Mutex_h

#include <iostream>
#include <pthread.h>

namespace mutex_module
{
    class Mutex
    {
    private:
        Mutex(const Mutex &m) = delete;
        Mutex operator=(const Mutex &m) = delete;

    public:
        Mutex()
        {
            pthread_mutex_init(&mutex_, NULL);
        }

        void lock()
        {
            pthread_mutex_lock(&mutex_);
        }

        void unlock()
        {
            pthread_mutex_unlock(&mutex_);
        }

        pthread_mutex_t *getLockAddress()
        {
            return &mutex_;
        }

        ~Mutex()
        {
            pthread_mutex_destroy(&mutex_);
        }

    private:
        pthread_mutex_t mutex_;
    };

    class MutexGuard
    {
    public:
        MutexGuard(Mutex &mutex)
            : mutex_(mutex)
        {
            // 创建对象时就自动上锁
            mutex_.lock();
        }

        ~MutexGuard()
        {
            // 销毁对象时就自动解锁
            mutex_.unlock();
        }

    private:
        Mutex &mutex_;
    };
}
#endif