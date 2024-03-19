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
static thread_local Fiber::ptr t_fiber = nullptr;

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

        // 将当前线程放入线程池中， 后续会参与调度
        t_fiber = m_rootFiber;
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



// 获得当前调度器
Scheduler::ptr Scheduler::GetThis(){
}
// 调度器的主协程
//      注意区分与线程的主协程的区别
Fiber::ptr Scheduler::GetMainFiber(){
}

void Scheduler::start(){
}
void Scheduler::stop(){
}

void Scheduler::run() {

}

void Scheduler::tickle() {

}



}

