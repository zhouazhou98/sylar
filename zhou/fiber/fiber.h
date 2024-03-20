#ifndef __ZHOU_FIBER_H__
#define __ZHOU_FIBER_H__

#include <ucontext.h>
#include <functional>
#include <memory>

#include "zhou/thread/thread.h"     // 线程模块
#include "zhou/thread/lock.h"       // mutex: 互斥
#include "zhou/thread/rwlock.h"     // 读写锁： 互斥
#include "zhou/thread/semaphore.h"  // 信号量： 同步

#include "stack_allocator.h"



namespace zhou {    // __ZHOU_FIBER_H__



// 注意： 继承了 enable_shared_from_this 的类不能在栈上创建实例
//      原因： 它必须是智能指针的成员
class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum STATUS {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT,
    };
private:
    Fiber();

public:
    Fiber(std::function<void()> callback, size_t stacksize = 0);
    ~Fiber();

    // 如果当前协程执行完成 或者 执行有问题 时， 我们可以将其状态/函数重置
    //      即： 已经创建好的协程的回收复用
    //      不需要重新分配协程的内存
    // INIT, TERM 状态的 协程 可以 reset
    void reset(std::function<void()> callback);

    // 切换进当前协程执行
    void swapIn();
    // 将当前协程切换到后台
    void swapOut();

public:
    static void SetThis(Fiber::ptr fiber);
    // 返回当前协程
    static Fiber::ptr GetThis();    /// static 方法： 报错 { return shared_from_this(); }
    // 协程切换到后台并设置为 Ready
    static void YeildToReady();
    // 协程切换到后台并设置为 Hold
    static void YeildToHold();
    // 总协程数
    static uint64_t TotalFiber();

    static uint64_t GetFiberId();

    
    static void MainFunc();

public:
    void setState(Fiber::STATUS state) { m_state = state; }
    Fiber::STATUS getState() { return m_state; }
private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    STATUS m_state = INIT;

    ucontext_t m_ctx;
    void * m_stack = nullptr;

    std::function<void()>  m_callback;
};

}       // ! __ZHOU_FIBER_H__


#endif  // ! __ZHOU_FIBER_H__
