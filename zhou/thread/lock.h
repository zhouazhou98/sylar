#ifndef     __ZHOU_LOCK_H__
#define     __ZHOU_LOCK_H__

#include <pthread.h>

namespace {     // ScopedLockImpl

template <class T>
class ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.lock();
        m_locked = true;
    }
    ~ScopedLockImpl() {
        unlock();
    }

    void lock() {
        if(!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }
    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T & m_mutex;
    bool m_locked;
};

}       // !ScopedLockImpl


namespace zhou {    // Mutex
class Mutex {
public:

private:
    pthread_mutex_t m_lock;
};

}   // ! Mutex



namespace zhou {    // RWMutex
class RWMutex {
public:
    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock() {
        pthread_rwlock_rdlock(&m_lock);
    }
    void wrlock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_lock);

    }
private:
    pthread_rwlock_t m_lock;

};

}   // ! RWMutex

#endif      // !__ZHOU_LOCK_H__