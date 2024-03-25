#ifndef __ZHOU_TIMER_H__
#define __ZHOU_TIMER_H__

#include <memory>
#include <vector>
#include <functional>

namespace zhou {

class TimerManager;

class Timer : public std::enable_shared_from_this<Timer> {
friend class TimerManager;

public:
    typedef std::shared_ptr<Timer> ptr;

    // 取消定时器
    bool cancel(bool cancel_callback = true);
    // 刷新设置定时器的执行时间
    bool refresh();
    // 重置定时器时间
    bool reset(uint64_t ms, bool from_now);

private:
    // ms: 定时器执行间隔时间
    // callback: 回调函数
    // recurring: 是否循环
    // manager: 定时器管理器
    Timer(uint64_t ms, std::function<void()> callback, bool recurring, TimerManager * manager);

    // next: 执行的时间戳
    Timer(uint64_t next);

private:
    bool m_recurring = false;   // 是否循环定时器
    uint64_t m_ms = 0;          // 执行周期
    uint64_t m_next = 0;        // 精确的执行时间
    std::function<void()> m_callback; // 回调函数
    TimerManager * m_manager;   // 定时器管理器

private:
    struct Comparator {
        // 比较定时器智能指针的大小 (按照执行时间排序)
        bool operator() (const Timer::ptr & lhs, const Timer::ptr & rhs) const;
    };
};

}

#endif  // ! __ZHOU_TIMER_H__
