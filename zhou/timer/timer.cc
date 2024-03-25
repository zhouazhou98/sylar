
#include "timer.h"
#include "timer_manager.h"
#include "zhou/utils/util.h"


namespace zhou {

// <
bool Timer::Comparator::operator() (const Timer::ptr & lhs, const Timer::ptr & rhs) const {
    if (!lhs && !rhs) {
        return false;
    }
    if (!lhs) {
        return true;
    }
    if (!rhs) {
        return false;
    }

    if (lhs->m_next < rhs->m_next) {
        return true;
    } else if (lhs->m_next > rhs->m_next) {
        return false;
    }
    return lhs.get() < rhs.get();

}

// ms: 定时器执行间隔时间
// callback: 回调函数
// recurring: 是否循环
// manager: 定时器管理器
Timer::Timer(uint64_t ms, std::function<void()> callback, bool recurring, TimerManager * manager) 
        :   m_recurring(recurring), 
            m_ms(ms), 
            m_callback(callback),
            m_manager(manager) {
    m_next = GetCurrentMS() + m_ms;
}

// next: 执行的时间戳
Timer::Timer(uint64_t next) : m_next(next) {
}

// 取消定时器
//  从 定时器管理器 集合中找到当前定时器并将其从集合中删除
bool Timer::cancel(bool cancel_callback) {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (!m_callback) {
        return false;
    }
    if (cancel_callback) {
        m_callback = nullptr;
    }
    auto iter = m_manager->m_timers.find(shared_from_this());
    if (iter == m_manager->m_timers.end()) {
        return false;
    } 
    m_manager->m_timers.erase(iter);
    return true;
}
// 刷新设置定时器的执行时间
bool Timer::refresh() {
    // if (!m_callback) {
    //     return false;
    // }
    // auto iter = m_manager->m_timers.find(shared_from_this());
    // if (iter == m_manager->m_timers.end()) {
    //     return false;
    // } 
    // m_manager->m_timers.erase(iter);

    if (!cancel(0)) {
        return false;
    }

    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    m_next = GetCurrentMS() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}
// 重置定时器时间
bool Timer::reset(uint64_t ms, bool from_now) {
    if (!cancel(0)) {
        return false;
    }

    if (ms == m_ms && !from_now) {
        return true;
    }

    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);

    uint64_t start = 0;
    if (from_now) {
        start = GetCurrentMS();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;
    m_manager->addTimer(shared_from_this(), lock);
    return true;
}

}
