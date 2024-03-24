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
    
    TimerManager();
    virtual ~TimerManager();

    // 添加定时器
    //  ms: 定时器执行间隔时间
    //  callback: 回调函数
    //  recurring: 是否循环
    Timer::ptr addTimer(uint64_t ms, std::function<void()> callback, bool recurring = false);

    // 添加条件定时器
    //  weak_condition: 条件
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> callback, std::weak_ptr<void> weak_condition, bool recurring = false);

    // 到最近一个将要执行的定时器执行的时间间隔
    uint64_t getNextTimer();

    // 获取需要执行的定时器的回调函数列表
    void listExpiredCallback(std::vector<std::function<void()>> & callbacks);

    // 检查是否有定时器
    bool hasTime();


protected:
    // 添加定时器
    //  val: 定时器
    //  lock: 锁
    void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);

    // 当有新的定时器插入到定时器的首部，执行该函数
    virtual void onTimerInsertedAtFront() = 0;

private:
    // 检测服务器时间是否被调后了
    bool detectClockRollover(uint64_t now_ms);
private:
    RWMutexType m_mutex;                                // 锁
    std::set<Timer::ptr, Timer::Comparator> m_timers;   // 定时器集合
    bool m_tickled = false;                             // 是否触发 onTimerInsertedAtFront
    uint64_t m_previousTime = 0;                        // 上次执行时间
};

}


#endif  // ! __ZHOU_TIMER_MANAGER_H__
