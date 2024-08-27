#include "hook.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"
#include "zhou/hook/fd_ctx.h"
#include "zhou/hook/fd_manager.h"
#include "zhou/fiber/scheduler.h"
#include "zhou/fiber/iomanager.h"
#include <errno.h>


static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

#ifdef __cplusplus
extern "C" {
#endif

#define XX(name) name ## _fun_p name ## _hook = nullptr;
    XX(socket)
    XX(connect)
    XX(accept)
#undef XX


// 2. -------------- socket --------------
// 2.1 socket
// int socket(int domain, int type, int protocol);
int socket(int domain, int type, int protocol) {
    if (!zhou::is_hook_enable()) {
        return socket_hook(domain, type, protocol);
    }
    int sock_fd = socket_hook(domain, type, protocol);
    if (sock_fd == -1) {
        return sock_fd;
    }
    zhou::SingleFDManager::GetInstance()->get(sock_fd, true);
    return 0;
}

// 2.2 connet
int  connect_with_timeout(int sockfd, const struct sockaddr * addr, socklen_t addrlen, uint64_t timeout_ms) {
    // 1. 获取 FdCtx
    zhou::FDCtx::ptr ctx = zhou::SingleFDManager::GetInstance()->get(sockfd);
    if (!ctx || ctx->isClosed()) {
        errno = EBADF;
        return -1;
    }

    // 2. 处理非 socket fd
    if (!ctx->isSocket()) {
        return connect_hook(sockfd, addr, addrlen);
    }

    // 3. 处理非阻塞 socket (如果用户设置非阻塞，则不需要我们 hook 后的函数将其更改为非阻塞)（如果用户不设置非阻塞，则为了效率，我们需要使用 hook 后的函数，将其改为非阻塞）
    if (!ctx->getUserNonblock()) {
        return connect_hook(sockfd, addr, addrlen);
    }

    // 4. 调用原始 connect 函数处理阻塞 socket 连接
    int n = connect_hook(sockfd, addr, addrlen);
    if (n == 0) {   // 连接立即成功
        return 0;
    } else if (n != -1 && errno != EINPROGRESS) {   // 连接失败，且返回值不是 -1（连接失败），且 errno 不是 EINPROGRESS（正在连接）
        // 连接失败：需要
        // errno: 失败原因是 EINPROGRESS 正在连接
        return n;
    }

    // 5. 处理由于“正在连接”而失败的请求
    // 5.1 IO 管理器 与 Timer 定时器
    zhou::IOManager * iom = zhou::IOManager::GetThis();
    zhou::Timer::ptr timer;
    timer_info::ptr timerinfo(new timer_info);
    timer_info::w_ptr w_timerinfo(timerinfo);

    // 5.2 设置超时定时器（超时取消）
    if (timeout_ms != (uint64_t)-1) {
        // 超过 timeout_ms 时间后取消该事件
        timer = iom->addConditionTimer(timeout_ms, [w_timerinfo, sockfd, iom]() {
            auto t = w_timerinfo.lock();
            if (!t || t->is_cancled) {
                return;
            }
            t->is_cancled = ETIMEDOUT;
            iom->cancelEvent(sockfd, zhou::IOManager::WRITE);
        }, w_timerinfo);
    }

    // 5.3 添加事件监听 并 处理连接
    int rt = iom->addEvent(sockfd, zhou::IOManager::WRITE);
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
    } else {        // 事件注册失败
        if (timer) {
            timer->cancel();
        }
        ZHOU_ERROR(g_logger) << "connect addEvent(socket fd: " << sockfd << ", WRITE) error";
        return -1;
    }

    // 5.4 检查 “正在连接” 的 socket fd 是否已经正常连接
    int error = 0;
    socklen_t len = sizeof(int);
    // 获取 socket 的错误状态，写入 error 中
    if (-1 == getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len)) {
        return -1;
    }
    // 若正确连接则返回 0
    if(!error) {
        return 0;
    } else {    // 否则继续返回 -1，并设置 errno
        errno = error;
        return -1;
    }

}


// int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (!zhou::is_hook_enable()) {
        return connect_hook(sockfd, addr, addrlen);
    }

    return connect_with_timeout(sockfd, addr, addrlen, zhou::get_s_connect_timeout());
}


// 2.3 accept
// int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int fd = zhou::do_io(sockfd, accept_hook, "accept", zhou::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
    if (fd >= 0) {
        zhou::SingleFDManager::GetInstance()->get(sockfd, true);
    }
    return fd;
}



#ifdef __cplusplus
}
#endif
