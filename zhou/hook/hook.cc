#include "hook.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"


namespace zhou {

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");

static thread_local bool t_hook_enable = true;

// 当前线程是否开启 hook
bool is_hook_enable() {
    return t_hook_enable;
}

// 设置当前线程的 hool 状态
void set_hook_enable(bool flag) {
    t_hook_enable = flag;
}

}


#include <dlfcn.h>

// main 函数前调用
namespace zhou {

#define HOOK_ALL_XX(XX) \
    XX(sleep)           \
    XX(usleep)          \
    XX(nanosleep)       \
                        \
    XX(socket)          \
    XX(connect)         \
    XX(accept)          



void hook_init() {
    static bool is_inited = false;
    if (is_inited) {
        return;
    }
// 将 函数指针 hook
#define XX(name) name ## _hook = (name ## _fun_p)dlsym(RTLD_NEXT, #name);
    HOOK_ALL_XX(XX)
#undef XX
} 

struct _HookIniterBeforeMain {
    _HookIniterBeforeMain() {
        hook_init();
    }
};

static _HookIniterBeforeMain s_hook_initer;

}
