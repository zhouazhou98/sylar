#ifndef     __ZHOU_LOCK_H__
#define     __ZHOU_LOCK_H__

#include <pthread.h>

namespace zhou {     // ScopedLockImpl

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

#endif      // !__ZHOU_LOCK_H__
