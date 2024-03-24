#ifndef __ZHOU_TIMER_MANAGER_H__
#define __ZHOU_TIMER_MANAGER_H__

#include <set>
#include "timer.h"
#include "zhou/thread/rwlock.h"

namespace zhou {

class Timer;

class TimerManager {
friend class Timer;

public:
    typedef RWMutex RWMutexType;
    void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);

private:
    std::set<Timer::ptr, Timer::Comparator> m_timers;
    RWMutexType m_mutex;
};

}


#endif  // ! __ZHOU_TIMER_MANAGER_H__
