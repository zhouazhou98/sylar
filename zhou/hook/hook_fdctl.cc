#include "hook.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"
#include "zhou/fiber/fiber.h"
#include "zhou/fiber/iomanager.h"
#include "zhou/fiber/scheduler.h"
#include "zhou/hook/fd_ctx.h"
#include "zhou/hook/fd_manager.h"


static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

#ifdef __cplusplus
extern "C" {
#endif

#define XX(name) name ## _fun_p name ## _hook = nullptr;
    XX(close)   \
    XX(fcntl)   \
    XX(ioctl)
#undef XX


// 5. -------------- fd control function --------------
// 5.1 close
int close(int fd) {
    if (zhou::is_hook_enable()) {
        return close_hook(fd);
    }

    zhou::FDCtx::ptr ctx = zhou::SingleFDManager::GetInstance()->get(fd);
    if (ctx) {
        zhou::IOManager * iom = zhou::IOManager::GetThis();
        if (iom) {
            iom->cancelAll(fd);
        }
        zhou::SingleFDManager::GetInstance()->del(fd);
    }
    return close_hook(fd);
}

// 5.2 fcntl
int fcntl(int fd, int cmd, ... /* arg */ ) {
    va_list va;
    va_start(va, cmd);
    switch(cmd) {
        case F_SETFL:
            {
                int arg = va_arg(va, int);
                va_end(va);
                zhou::FDCtx::ptr ctx = zhou::SingleFDManager::GetInstance()->get(fd);
                if(!ctx || ctx->isClosed() || !ctx->isSocket()) {
                    return fcntl_hook(fd, cmd, arg);
                }
                ctx->setUserNonblock(arg & O_NONBLOCK);
                if(ctx->getSysNonblock()) {
                    arg |= O_NONBLOCK;
                } else {
                    arg &= ~O_NONBLOCK;
                }
                return fcntl_hook(fd, cmd, arg);
            }
            break;
        case F_GETFL:
            {
                va_end(va);
                int arg = fcntl_hook(fd, cmd);
                zhou::FDCtx::ptr ctx = zhou::SingleFDManager::GetInstance()->get(fd);
                if(!ctx || ctx->isClosed() || !ctx->isSocket()) {
                    return arg;
                }
                if(ctx->getUserNonblock()) {
                    return arg | O_NONBLOCK;
                } else {
                    return arg & ~O_NONBLOCK;
                }
            }
            break;
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
        case F_SETPIPE_SZ:
            {
                int arg = va_arg(va, int);
                va_end(va);
                return fcntl_hook(fd, cmd, arg); 
            }
            break;
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
        case F_GETPIPE_SZ:
            {
                va_end(va);
                return fcntl_hook(fd, cmd);
            }
            break;
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
            {
                struct flock* arg = va_arg(va, struct flock*);
                va_end(va);
                return fcntl_hook(fd, cmd, arg);
            }
            break;
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            {
                struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
                va_end(va);
                return fcntl_hook(fd, cmd, arg);
            }
            break;
        default:
            va_end(va);
            return fcntl_hook(fd, cmd);
    }
    return 0;
}

// 5.3 ioctl
int ioctl(int fd, unsigned long request, ...) {
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);

    if(FIONBIO == request) {
        bool user_nonblock = !!*(int*)arg;
        zhou::FDCtx::ptr ctx = zhou::SingleFDManager::GetInstance()->get(fd);
        if(!ctx || ctx->isClosed() || !ctx->isSocket()) {
            return ioctl_hook(fd, request, arg);
        }
        ctx->setUserNonblock(user_nonblock);
    }
    return ioctl_hook(fd, request, arg);
}

#ifdef __cplusplus
}
#endif