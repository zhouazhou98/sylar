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
    if (!zhou::is_hook_enable()) {
        return sleep_hook(seconds);
    }

    ZHOU_INFO(g_logger) << "sleep hook";
    zhou::IOManager * iom = zhou::IOManager::GetThis(); // 获取当前线程的 IO 管理器

    // lambda 简化代码
    iom->addTimer(seconds * 1000, [iom]() {
            iom->schedule(zhou::Fiber::GetThis(), -1);
        }
    );

    return 0;
}

// 1.2 usleep 微秒
// int usleep(useconds_t usec);
int usleep(useconds_t usec) {
    if (!zhou::is_hook_enable()) {
        return usleep_hook(usec);
    }

    ZHOU_INFO(g_logger) << "usleep hook";
    zhou::IOManager * iom = zhou::IOManager::GetThis(); // 获取当前线程的 IO 管理器

    // lambda 简化代码
    iom->addTimer(usec / 1000, [iom]() {
            iom->schedule(zhou::Fiber::GetThis(), -1);
        }
    );

    return 0;
}

// 1.3 nanosleep
int nanosleep(const struct timespec *req, struct timespec *rem) {
    if (!zhou::is_hook_enable()) {
        return nanosleep_hook(req, rem);
    }

    ZHOU_INFO(g_logger) << "nanosleep hook";
    zhou::IOManager * iom = zhou::IOManager::GetThis(); // 获取当前线程的 IO 管理器

    int timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000 /1000;

    // lambda 简化代码
    iom->addTimer(timeout_ms, [iom]() {
            iom->schedule(zhou::Fiber::GetThis(), -1);
        }
    );

    return 0;
}

#ifdef __cplusplus
}
#endif
