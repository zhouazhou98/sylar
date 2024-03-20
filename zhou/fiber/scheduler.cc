#include "scheduler.h"
#include "fiber.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"


static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");

namespace zhou {

// 要在当前线程中获取当前协程调度器的指针
static thread_local Scheduler::ptr t_scheduler = nullptr;
// 声明当前协程的主协程 ID 
static thread_local Fiber::ptr t_scheduler_fiber = nullptr;

Scheduler::Scheduler(int thread_count, bool use_caller, const std::string & name) 
        : m_name(name) {
    ZHOU_ASSERT(thread_count > 0);

    // 是否使用调用该初始化函数的线程作为线程池中被调度的其中之一
    if (use_caller) {
        // 当前线程如果没有主协程，则创建一个主协程
        Fiber::GetThis();
        --thread_count;

        // 说明当前线程还没有创建过 协程调度器 Scheduler
        ZHOU_ASSERT(GetThis() == nullptr);
// t_scheduler = shared_from_this();
        // m_rootFiber.reset((new Fiber(Scheduler::run)));
        m_rootFiber.reset(  new Fiber(
                                std::bind(
                                    &Scheduler::run, this
                                )
                        ));
        Thread::SetName(m_name);

        // 将当前线程放入线程池中， 后续会参与调度（被调度）
        t_scheduler_fiber = m_rootFiber;                  // 设置线程局部变量 t_scheduler_fiber 为调度器主协程
        m_rootThreadId = syscall(SYS_gettid);
        m_threadIds.push_back(m_rootThreadId);
    } else {
        m_rootThreadId = -1;
    }

    m_threadCount = thread_count;

}
Scheduler::~Scheduler(){
    ZHOU_ASSERT(m_stopping);
    if (GetThis().get() == this) {
        t_scheduler.reset();
    }
}



void Scheduler::setThis() {
    t_scheduler = shared_from_this();
}

// 获得当前调度器
Scheduler::ptr Scheduler::GetThis(){
    return t_scheduler;
}
// 调度器的主协程
//      注意区分与线程的主协程的区别
Fiber::ptr Scheduler::GetMainFiber(){
    return t_scheduler_fiber;
}



// 核心： 创建对应数量的线程， start() 执行完成后， 等待接下来需要处理的 协程 或 函数 进行 schedule
void Scheduler::start(){
    t_scheduler = shared_from_this();
    MutexType::Lock lock(m_mutex);

    if (!m_stopping) return;
    m_stopping = false;

    // 创建线程
    ZHOU_ASSERT(m_threads.empty());
    m_threads.resize(m_threadCount);
    for (size_t i = 0; i < m_threadCount; i++) {
        m_threads.push_back(
            Thread::ptr(new Thread(
                std::bind(&Scheduler::run, this)
                , m_name + "_" + std::to_string(i)
            ))
        );
        m_threadIds.push_back(m_threads[i]->getId());
    }
    lock.unlock();
}


// 1. 将线程全部唤醒， 执行未完成的任务（即 未完成的 [协程] 或 [函数]）
// 2. 由各个被调度的线程来执行调度器分配的 [协程] 或 [函数]
// 3. 等待所有子线程执行完毕， 即 所有 [协程] 或 [函数] 都执行完成， 线程 join 后退出
void Scheduler::stop() {
    m_autoStop = true;
    if (m_rootFiber
            && m_threadCount == 0
            && (m_rootFiber->getState() == Fiber::TERM
                    || m_rootFiber->getState() == Fiber::INIT
                    || m_rootFiber->getState() == Fiber::EXCEPT)
    ) {
        ZHOU_INFO(g_logger) << this << "stopped";
        m_stopping = true;

        if (stopping()) {
            return;
        }
    }

    // bool exist_on_this_fiber = false;
    if (m_rootThreadId == -1) {
        ZHOU_ASSERT(GetThis().get() == this)
    } else {
        ZHOU_ASSERT(GetThis().get() != this)
    }
    m_stopping = true;
    for (size_t i = 0; i < m_threadCount; i++) {
        tickle();
    }
    if (m_rootFiber) {
        tickle();
    }

    if (m_rootFiber) {
        if (!stopping()) {
            return;
        }
    }
}


// run(): 协调协程和线程的关系
// 所有由调度器创建的线程， 其执行的都是该方法
//      while 循环： 让所有的线程一直运行， 直到调度器给出命令让他们退出后， 它们才可以跳出循环
void Scheduler::run() {
    // 设置当前线程的调度器为创建该线程的 scheduler
    //      主要是靠 std::bind 函数来传入 scheduler 指针
    setThis();

    if ( syscall(SYS_gettid) != m_rootThreadId ) {
        t_scheduler_fiber = Fiber::GetThis();
    }

    Fiber::ptr idle_fiber(new Fiber(
                                std::bind(&Scheduler::idle, this)
    ));

    Fiber::ptr callback_fiber;
    FiberAndFunc fc;
    while (true) {
        fc.reset();     // 调用成员函数将内部成员变量重置
        bool tickle_me = false;
        {
            MutexType::Lock lock(m_mutex);
            for (auto iter = m_fibers.begin(); iter != m_fibers.end(); iter++) {
                if ((*iter)->thread_id != -1 && (*iter)->thread_id != syscall(SYS_gettid)) {
                    // 该 [协程] 或 [函数] 已经指定了其必须在哪个线程中执行
                    tickle_me = true;       // 该线程不需要处理这个 [协程] 或 [函数]， 但需要通知其它线程去处理
                    continue;
                }
                // 执行到这里： 
                //      1. [协程] [函数] 没有指定必须要运行在哪个线程上； 
                //      2. [协程] [函数] 指定了必须运行在哪个线程上，并且检验后发现当前线程即为指定的线程
                ZHOU_ASSERT((*iter)->callback || (*iter)->fiber);
                if ((*iter)->fiber && (*iter)->fiber->getState() == Fiber::EXEC) {
                    continue;
                }

                fc = **iter;
                m_fibers.erase(iter);
                ++m_activeThreadCount;
                
            }
        }

        if (tickle_me) {
            tickle();
        }

        if (fc.fiber 
            && (        (fc.fiber->getState() != Fiber::TERM)
                    &&  (fc.fiber->getState() != Fiber::EXCEPT)
                )
        ) {
            // 1. 执行
            fc.fiber->swapIn();
            --m_activeThreadCount;
            // 2. 检查执行后状态
            if (fc.fiber->getState() == Fiber::READY) {
                schedule(fc.fiber);
            } else if (
                        (fc.fiber->getState() != Fiber::TERM)
                    &&  (fc.fiber->getState() != Fiber::EXCEPT)
            ) {
                fc.fiber->setState(Fiber::HOLD);
            }
            // 3. 重置 fc
            fc.reset();
        }
        else if (fc.callback) {
            // 1. 为函数创建一个协程
            if (callback_fiber) {
                callback_fiber->reset(fc.callback);
            } else {
                callback_fiber.reset(new Fiber(fc.callback));
                fc.callback = nullptr;
            }
            // 2. 执行
            callback_fiber->swapIn();
            --m_activeThreadCount;
            // 3. 检查执行后状态
            if (callback_fiber->getState() == Fiber::READY) {
                schedule(callback_fiber);
                callback_fiber.reset();
            } else if (
                        (callback_fiber->getState() == Fiber::TERM)
                    ||  (callback_fiber->getState() == Fiber::EXCEPT)
            ) {
                callback_fiber->reset(nullptr);
            } else {
                callback_fiber->setState(Fiber::HOLD);
                callback_fiber.reset();
            }

            fc.reset();
        } else {
            if (idle_fiber->getState() == Fiber::TERM) {
                break;
            }
            ++m_idleThreadCount;
            idle_fiber->swapIn();
            if (
                        (fc.fiber->getState() != Fiber::TERM)
                    &&  (fc.fiber->getState() != Fiber::EXCEPT)
            ) {
                idle_fiber->setState(Fiber::HOLD);
            }
            --m_idleThreadCount;

        }
    }
}

// 让 Scheduler 子类有其它的清理任务的机会
bool Scheduler::stopping() {
    MutexType::Lock lock(m_mutex);
    return (
        m_autoStop &&
        m_stopping &&
        m_fibers.empty() &&
        m_activeThreadCount == 0
    );

}

void Scheduler::tickle() {
    ZHOU_INFO(g_logger) << "tickle";
}

void Scheduler::idle() {
    ZHOU_INFO(g_logger) << "idle";
    while (!stopping()) {
        zhou::Fiber::YeildToHold();
    }
}



}

