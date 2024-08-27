#include "hook.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"
#include "zhou/fiber/fiber.h"
#include "zhou/fiber/iomanager.h"
#include "zhou/fiber/scheduler.h"


static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

#ifdef __cplusplus
extern "C" {
#endif

#define XX(name) name ## _fun_p name ## _hook = nullptr;
    XX(sleep)
    XX(usleep)
    XX(nanosleep)
#undef XX



// 1. -------------- sleep --------------
// 使用 IOManager 来实现 epoll 超时调用
// 1.1 sleep 毫秒
unsigned int sleep(unsigned int seconds) {
    ZHOU_INFO(g_logger) << "sleep hook";
    // if (!zhou::is_hook_enable()) {
    //     return sleep_hook(seconds);
    // }

    zhou::Fiber::ptr fiber = zhou::Fiber::GetThis();    // 获取当前线程运行的协程
    zhou::IOManager * iom = zhou::IOManager::GetThis(); // 获取当前线程的 IO 管理器

    ZHOU_INFO(g_logger) << "sleep hook";
    // iom->addTimer(
    //     seconds * 1000, 
    //     std::bind(          // 生成一个函数对象，该对象可以将成员函数与对象实例绑定在一起
    //         ( void (zhou::Scheduler::*) (zhou::Fiber::ptr, int thread) )    // 将 zhou::Scheduler::schedule 地址看作 void (zhou::Scheduler::*) (zhou::Fiber::ptr, int thread)
    //                 & zhou::Scheduler::schedule,
    //         iom,        // iom 对象实例  ---> 由于 zhou::Scheduler 是 zhou::IOManager 基类，因此这里用了泛型
    //         fiber,      // 要调度的协程
    //         -1          // 选择任意线程
    //     )
    // );
    // lambda 简化代码
    iom->addTimer(seconds * 1000, [iom]() {
            ZHOU_INFO(g_logger) << "scheuling...";
            iom->schedule(zhou::Fiber::GetThis(), -1);
            ZHOU_INFO(g_logger) << "scheuling... end";
        }
    );


    ZHOU_INFO(g_logger) << "sleep hook end";

    ZHOU_INFO(g_logger) << "swap in";
    return 0;
}

// 1.2 usleep 微秒
// int usleep(useconds_t usec);
int usleep(useconds_t usec) {
    ZHOU_INFO(g_logger) << "usleep hook";
    return 0;
}

// 1.3 nanosleep
int nanosleep(const struct timespec *req, struct timespec *rem) {
    ZHOU_INFO(g_logger) << "nanosleep hook";
    return 0;
}

#ifdef __cplusplus
}
#endif
