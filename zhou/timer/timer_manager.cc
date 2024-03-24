
#include "timer_manager.h"
#include "zhou/utils/util.h"

namespace zhou {

TimerManager::TimerManager(){
    m_previousTime = GetCurrentMS();
}
TimerManager::~TimerManager(){
}

// 添加定时器
//  ms: 定时器执行间隔时间
//  callback: 回调函数
//  recurring: 是否循环
Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> callback, bool recurring){
    Timer::ptr timer(new Timer(ms, callback, recurring, this));
    RWMutexType::WriteLock lock(m_mutex);
    addTimer(timer, lock);
    return timer;
}

static void OnTimer(std::weak_ptr<void> weak_condition, std::function<void()> callback) {
    std::shared_ptr<void> tmp = weak_condition.lock();
    if (tmp) {
        callback();
    }
}

// 添加条件定时器
//  weak_condition: 条件
Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> callback, std::weak_ptr<void> weak_condition, bool recurring){
    return addTimer(ms, std::bind(&OnTimer, weak_condition, callback), recurring);
}

// 到最近一个将要执行的定时器执行的时间间隔
uint64_t TimerManager::getNextTimer(){
    RWMutexType::ReadLock lock(m_mutex);
    m_tickled = false;
    if (m_timers.empty()) {
        return ~0ull;
    }

    // 获得下一次定时器触发距离现在的时间
    const Timer::ptr & next = *m_timers.begin();
    uint64_t now_ms = GetCurrentMS();
    if (now_ms >= next->m_next) {
        return 0;
    } else {
        return next->m_next - now_ms;
    }
}

// 获取需要执行的定时器的回调函数列表
void TimerManager::listExpiredCallback(std::vector<std::function<void()>> & callbacks){
    uint64_t now_ms = GetCurrentMS();
    std::vector<Timer::ptr> expired;
    {
        RWMutexType::ReadLock lock(m_mutex);
        if (m_timers.empty()) {
            return;
        }
    }

    RWMutexType::WriteLock lock(m_mutex);
    if (m_timers.empty()) {
        return;
    }
    bool rollover = detectClockRollover(now_ms);
    if (!rollover && ((*m_timers.begin())->m_next > now_ms)) {
        return;
    }

    Timer::ptr now_timer(new Timer(now_ms));
    auto iter = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    while (iter != m_timers.end() && ((*iter)->m_next == now_ms)) {
        ++iter;
    }

    expired.insert(expired.begin(), m_timers.begin(), iter);
    callbacks.reserve(expired.size());

    for (auto & timer : expired) {
        callbacks.push_back(timer->m_callback);
        if (timer->m_recurring) {
            timer->m_next = now_ms + timer->m_ms;
            m_timers.insert(timer);
        } else {
            timer->m_callback = nullptr;
        }
    }
}

// 检查是否有定时器
bool TimerManager::hasTime(){
    RWMutexType::ReadLock lock(m_mutex);
    return !m_timers.empty();
}


// 添加定时器
//  val: 定时器
//  lock: 锁
void TimerManager::addTimer(Timer::ptr timer, RWMutexType::WriteLock& lock){
    auto iter = m_timers.insert(timer).first;
    bool at_front = (iter == m_timers.begin()) && !m_tickled;
    if (at_front) {
        m_tickled = true;
    }
    lock.unlock();

    if (at_front) {
        onTimerInsertedAtFront();
    }
}


// 检测服务器时间是否被调后了
bool TimerManager::detectClockRollover(uint64_t now_ms){
    bool rollover = false;
    if (now_ms < m_previousTime && now_ms < (m_previousTime - 60 * 60 * 1000)) {
        rollover = true;
    }
    
    m_previousTime = now_ms;
    return rollover;
}





}


