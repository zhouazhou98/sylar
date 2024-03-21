
#include "zhou/utils/macro.h"
#include "zhou/utils/util.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "fiber.h"
#include <atomic>

zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

namespace zhou {

static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

// 对于每个线程来说都有一个主协程，这里使用线程局部变量来表示主协程
// 当前线程运行的是哪个协程
static thread_local Fiber::ptr t_fiber = nullptr;
// 当前线程的主协程 main fiber
static thread_local Fiber::ptr t_main_fiber = nullptr;

static uint32_t g_fiber_stacksize = 1024 * 1024;


// 如果不想用 mallo 分配，可以自行更改栈分配函数
typedef MallocStackAllocator StackAllocator;

// -------------------  Fiber  -----------------------

// private 私有初始化函数，该函数只会被每个线程调用一次用来创建线程的 main fiber，之后都是各个线程中的 main fiber 来创建/管理/调度 子协程
//      因此该函数创建的协程 ID 一定是 0
//  外部如何调用到该 private 构造函数： 通过 Fiber 类内部的 public 静态成员函数来调用 private: Fiber() 构造函数
Fiber::Fiber() {
    // 第一个协程一定是线程的主协程
    m_state = EXEC;

    if ( getcontext(&this->m_ctx) ) {
        ZHOU_ASSERT2(0, "getcontext");
    }

    ++s_fiber_count;
    ZHOU_DEBUG(g_logger) << "Fiber::Fiber() main id = " << m_id;

}
// public 初始化
Fiber::Fiber(std::function<void()> callback, size_t stacksize)
        :   m_id(++s_fiber_id),
            m_callback(callback) {
    if (!t_main_fiber) {
        t_main_fiber = zhou::Fiber::ptr(new zhou::Fiber);
    }
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stacksize;

    m_stack = StackAllocator::Alloc(m_stacksize);

    if ( getcontext(&this->m_ctx) ) {
        ZHOU_ASSERT2(0, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&this->m_ctx, &MainFunc, 0);
    ZHOU_DEBUG(g_logger) << "Fiber::Fiber id = " << m_id;
}
Fiber::~Fiber() {
    --s_fiber_count;
    if (m_stack) {
        ZHOU_ASSERT(m_state == INIT || m_state == TERM || m_state == EXCEPT);

        StackAllocator::Dealloc(m_stack, m_stacksize);
    } else {
        if (t_main_fiber.get() != this) {
            ZHOU_ASSERT(m_callback);
            ZHOU_ASSERT(m_state == EXEC);
        }
        if (t_fiber.get() == this) {
            SetThis(nullptr);
        }
    }

    // if (t_main_fiber.get() == this) {
    //     ZHOU_DEBUG(g_logger) << "Fiber::~Fiber main fiber";
    //     t_fiber.reset();
    // }

    ZHOU_DEBUG(g_logger) << "Fiber::~Fiber id = " << m_id;
}
void Fiber::reset(std::function<void()> callback) {
    ZHOU_ASSERT(m_stack);
    ZHOU_ASSERT(m_state == INIT || m_state == TERM || m_state == EXCEPT);
    m_callback = callback;


    if ( getcontext(&this->m_ctx) ) {
        ZHOU_ASSERT2(0, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&this->m_ctx, &MainFunc, 0);
    m_state = INIT;
}

// 切换进当前协程执行
//      目前我理解的： 从主协程切换到当前协程
void Fiber::swapIn(ucontext_t * exit_ctx) {
    SetThis(shared_from_this());
    // 需要切换到的这个协程当前一定没在执行， 所以需要调度执行它
    ZHOU_ASSERT(m_state != EXEC);

    m_state = EXEC;

    if (exit_ctx) {
        m_exit_ctx = exit_ctx;
        if (swapcontext(exit_ctx, &this->m_ctx)) {
            ZHOU_ASSERT2(0, "swapcontext");
        }
    } else {
        if (swapcontext(&t_main_fiber->m_ctx, &this->m_ctx)) {
            ZHOU_ASSERT2(0, "swapcontext");
        }
    }


}
// 将当前协程切换到后台， 将 CPU 控制权交换给 main fiber 进行后续调度
void Fiber::swapOut() {
    SetThis(t_main_fiber);

    if (m_state == EXEC) {
        m_state = HOLD;
    }
    
    ZHOU_ASSERT(m_state == EXEC || m_state == READY || m_state == HOLD || m_state == TERM || m_state == EXCEPT);

    // 由于该函数被 Yeild 调用，会在 Yeild 函数中设置其 m_state ，这里不再进行设置
    // m_state = HOLD;
    if (m_exit_ctx) {
        if (swapcontext(&this->m_ctx, m_exit_ctx)) {
            ZHOU_ASSERT2(0, "swapcontext");
        }
    } else {
        if (swapcontext(&this->m_ctx, &t_main_fiber->m_ctx)) {
            ZHOU_ASSERT2(0, "swapcontext");
        }
    }
}

    

// ------------- static 方法 -------------
void Fiber::SetThis(Fiber::ptr fiber) {
    if (!fiber) {
        t_fiber.reset();
    }
    t_fiber.swap(fiber);

}
// 返回当前协程
Fiber::ptr Fiber::GetThis() {
    if (!t_fiber) {
        if (t_main_fiber) ZHOU_ASSERT2(0, "main fiber exist but current fiber don't exist");

        Fiber::ptr main_fiber(new Fiber);
        SetThis(main_fiber);
        ZHOU_ASSERT(t_fiber == main_fiber);
        t_main_fiber = main_fiber;
        return t_main_fiber;
    }
    return Fiber::ptr(t_fiber);
}
// 协程切换到后台并设置为 Ready
void Fiber::YeildToReady() {
    Fiber::ptr curr = Fiber::GetThis();
    // 只有子协程才会调用该方法
    ZHOU_ASSERT2(curr != t_main_fiber, "main fiber yeild");
    curr->m_state = READY;
    curr->swapOut();
}
// 协程切换到后台并设置为 Hold
void Fiber::YeildToHold() {
    Fiber::ptr curr = Fiber::GetThis();
    // 只有子协程才会调用该方法
    ZHOU_ASSERT2(curr != t_main_fiber, "main fiber yeild");
    curr->m_state = HOLD;
    curr->swapOut();
}
// 总协程数
uint64_t Fiber::TotalFiber() {
    return s_fiber_count;
}



// 如果直接使用 GetThis 方法，有的线程并没有协程，也会被迫生成主协程
uint64_t Fiber::GetFiberId() {
    if (t_fiber) {
        return t_fiber->m_id;
    }
    return 0;
}
    
void Fiber::MainFunc() {
    Fiber::ptr curr = Fiber::GetThis();
    ZHOU_ASSERT(curr);

    try {
        curr->m_callback();
        curr->m_callback = nullptr;
        curr->m_state = TERM;
    } catch (std::exception &ex) {
        curr->m_state = EXCEPT;
        ZHOU_ERROR(zhou::SingleLoggerManager::GetInstance()->getLogger("system")) << "Fiber EXCEPT: " << ex.what();
    } catch (...) {
        curr->m_state = EXCEPT;
        ZHOU_ERROR(zhou::SingleLoggerManager::GetInstance()->getLogger("system")) << "Fiber EXCEPT";
    }
    
    Fiber * raw_ptr = curr.get();
    curr.reset();
    raw_ptr->swapOut();

    // 如果想要执行这一步，应该将上述的 swapOut 操作改为 ucontext_t uc_link 实现
    ZHOU_ASSERT2(false, "do not exec forever");
}


void Fiber::call() {
    SetThis(shared_from_this());
    m_state = EXEC;
    ZHOU_INFO(g_logger) << m_id;
    if(swapcontext(&t_main_fiber->m_ctx, &m_ctx)) {
        ZHOU_ASSERT2(false, "swapcontext");
    }
}

void Fiber::back() {
    SetThis(t_main_fiber);
    if(swapcontext(&m_ctx, &t_main_fiber->m_ctx)) {
        ZHOU_ASSERT2(false, "swapcontext");
    }
}

}


