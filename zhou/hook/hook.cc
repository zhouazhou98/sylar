#include "zhou/hook/hook.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"
#include "zhou/fiber/iomanager.h"
#include "zhou/hook/fd_ctx.h"
#include "zhou/hook/fd_manager.h"

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

namespace zhou {


static thread_local bool t_hook_enable = true;

// 当前线程是否开启 hook
bool is_hook_enable() {
    return t_hook_enable;
}

// 设置当前线程的 hool 状态
void set_hook_enable(bool flag) {
    t_hook_enable = flag;
}

static uint64_t s_connect_timeout = -1;
uint64_t get_s_connect_timeout() {
    return s_connect_timeout;
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
    XX(accept)          \
    XX(getsockopt)      \
    XX(setsockopt)      \
                        \
    XX(read)            \
    XX(readv)           \
    XX(recv)            \
    XX(recvfrom)        \
    XX(recvmsg)         \
                        \
    XX(write)           \
    XX(writev)          \
    XX(send)            \
    XX(sendto)          \
    XX(sendmsg)         \
                        \
    XX(close)           \
    XX(fcntl)           \
    XX(ioctl)


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




template<typename OriginFun, typename... Args>
ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
        uint32_t event, int timeout_so, Args&&... args) {
    // 0. 没有开启 hook 则使用原始函数
    if(!zhou::is_hook_enable()){
        return fun(fd, std::forward<Args>(args)...);
    }
    // 1. 获取 FdCtx
    zhou::FDCtx::ptr ctx = zhou::SingleFDManager::GetInstance()->get(fd);
    if(!ctx){
        return fun(fd, std::forward<Args>(args)...);
    }
    if (ctx->isClosed()) {
        errno = EBADF;
        return -1;
    }

    // 2. 处理非 socket fd
    if (!ctx->isSocket()) {
        return fun(fd, std::forward<Args>(args)...);
    }

    // 3. 处理非阻塞 socket (如果用户设置非阻塞，则不需要我们 hook 后的函数将其更改为非阻塞)（如果用户不设置非阻塞，则为了效率，我们需要使用 hook 后的函数，将其改为非阻塞）
    if (!ctx->getUserNonblock()) {
        return fun(fd, std::forward<Args>(args)...);
    }


    timer_info::ptr timerinfo(new timer_info);
retry:
    // 4. 调用原始 connect 函数处理阻塞 socket 连接
    int n = fun(fd, std::forward<Args>(args)...);

    while (n == -1 && errno == EINTR) {   // EINTR
        n = fun(fd, std::forward<Args>(args)...);
    }

    if (n == -1 && errno == EAGAIN) {   // 连接失败，且返回值不是 -1（连接失败），且 errno 不是 EINPROGRESS（正在连接）
        // 连接失败：重试
        // errno: 失败原因是 EAGAIN 

        // 5. 处理由于“正在连接”而失败的请求
        // 5.1 IO 管理器 与 Timer 定时器
        zhou::IOManager * iom = zhou::IOManager::GetThis();
        zhou::Timer::ptr timer;
        timer_info::w_ptr w_timerinfo(timerinfo);

        // 5.2 设置超时定时器（超时取消）
        uint64_t timeout_ms = ctx->getTimeout(timeout_so);
        if (timeout_ms != (uint64_t)-1) {
            // 超过 timeout_ms 时间后取消该事件
            timer = iom->addConditionTimer(timeout_ms, [w_timerinfo, fd, iom, event]() {
                auto t = w_timerinfo.lock();
                if (!t || t->is_cancled) {
                    return;
                }
                t->is_cancled = ETIMEDOUT;
                iom->cancelEvent(fd, (zhou::IOManager::Event)(event));  // event 为 WRITE or READ
            }, w_timerinfo);
        }

        // 5.3 添加事件监听 并 处理连接
        int rt = iom->addEvent(fd, (zhou::IOManager::Event)(event));
        if (rt == 0) {   // 事件注册成功
            zhou::Fiber::GetThis()->swapOut();
            // 监听到 WRITE 事件，则取消超时定时器
            if (timer) {
                timer->cancel();
            }
            if (timerinfo->is_cancled) {
                errno = timerinfo->is_cancled;
                return -1;
            }
            ZHOU_ASSERT(zhou::Fiber::GetThis()->getState() == zhou::Fiber::EXEC);
            goto retry;
        } else {        // 事件注册失败
            if (timer) {
                timer->cancel();
            }
            ZHOU_ERROR(g_logger) << hook_fun_name << " addEvent(fd: " << fd << ", " << event << ") error";
            return -1;
        }
    }
    return n;
}

// 显式实例化模板函数 
// accept
template ssize_t do_io<int (*)(int, sockaddr*, unsigned int*), sockaddr*&, unsigned int*&>(
    int, int (*)(int, sockaddr*, unsigned int*), const char*, unsigned int, int, sockaddr*&, unsigned int*&);

// read
template ssize_t do_io<long (*)(int, void*, unsigned long, int, sockaddr*, unsigned int*), void*&, unsigned long&, int&, sockaddr*&, unsigned int*&>(
    int, long (*)(int, void*, unsigned long, int, sockaddr*, unsigned int*), char const*, unsigned int, int, void*&, unsigned long&, int&, sockaddr*&, unsigned int*&);

template ssize_t do_io<long (*)(int, void*, unsigned long), void*&, unsigned long&>(
    int, long (*)(int, void*, unsigned long), char const*, unsigned int, int, void*&, unsigned long&);

template ssize_t do_io<long (*)(int, msghdr*, int), msghdr*&, int&>(
    int, long (*)(int, msghdr*, int), char const*, unsigned int, int, msghdr*&, int&);

template ssize_t do_io<long (*)(int, iovec const*, int), iovec const*&, int&>(
    int, long (*)(int, iovec const*, int), char const*, unsigned int, int, iovec const*&, int&);

template ssize_t do_io<long (*)(int, void*, unsigned long, int), void*&, unsigned long&, int&>(
    int, long (*)(int, void*, unsigned long, int), char const*, unsigned int, int, void*&, unsigned long&, int&);


// write
template long do_io<long (*)(int, void const*, unsigned long), void const*&, unsigned long&>
    (int, long (*)(int, void const*, unsigned long), char const*, unsigned int, int, void const*&, unsigned long&);

template long do_io<long (*)(int, void const*, unsigned long, int), void const*&, unsigned long&, int&>
    (int, long (*)(int, void const*, unsigned long, int), char const*, unsigned int, int, void const*&, unsigned long&, int&);

template long do_io<long (*)(int, msghdr const*, int), msghdr const*&, int&>
    (int, long (*)(int, msghdr const*, int), char const*, unsigned int, int, msghdr const*&, int&);

template long do_io<long (*)(int, void const*, unsigned long, int, sockaddr const*, unsigned int), void const*&, unsigned long&, int&, sockaddr const*&, unsigned int&>
    (int, long (*)(int, void const*, unsigned long, int, sockaddr const*, unsigned int), char const*, unsigned int, int, void const*&, unsigned long&, int&, sockaddr const*&, unsigned int&);

}