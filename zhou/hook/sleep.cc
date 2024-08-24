#include "hook.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"


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
