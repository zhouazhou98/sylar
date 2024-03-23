
#include "iomanager.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"
#include <memory>
#include <sys/epoll.h>      // epoll
#include <unistd.h>         // pipe
#include <string.h>         // memset
#include <fcntl.h>          // fcntl

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");

namespace zhou {
    // static thread_local IOManager::ptr t_io_manager;

// public

IOManager::IOManager(size_t thread_count, bool use_caller, const std::string & name) 
        : Scheduler(thread_count, use_caller, name) {
// 1. 创建 epoll 实例 并返回句柄 m_epfd
    // Since Linux 2.6.8, the size argument is [[ ignored ]], but must be greater than zero; see NOTES.
    m_epfd = epoll_create(5000);    // Linux 2.6.8 之后的内核会动态调整大小
    ZHOU_ASSERT(m_epfd > 0);

// 2. 线程通信 fd : 将管道加入 epoll_wait
    int rt = pipe(m_tickleFds);
    ZHOU_ASSERT(!rt);

    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];

    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    ZHOU_ASSERT(!rt);

    // 在 m_epfd 中加入 m_tickleFDs[0] 句柄，监听 event 事件 (EPOLLIN | EPOLLET) （读 | 边沿触发）
    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    ZHOU_ASSERT(!rt);

// 3. 启动： 创建线程，准备调度执行
    contextResize(32);
    start();
}

IOManager::~IOManager() {
    stop();
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);
    close(m_epfd);
}

// 1: success; 0: retry; -1: error
int IOManager::addEvent(int fd, Event event, std::function<void()> callback) {
    FdCtx::ptr fd_ctx;
    {
        RWMutexType::ReadLock lock(m_mutex);
        if ((int)m_fdCtxs.size() > fd) {
            fd_ctx = m_fdCtxs[fd];
        } else {
            RWMutexType::WriteLock lock2(m_mutex);
            lock.unlock();
            contextResize(m_fdCtxs.size() * 1.5);
            fd_ctx = m_fdCtxs[fd];
        }
    }

    FdCtx::MutexType::Lock lock(fd_ctx->mutex);
    // 如果 fd_ctx 中已经监听了 event 事件， 则这次添加是有问题的
    if (fd_ctx->events & event) {
        ZHOU_ERROR(g_logger) << "";

    }
    return 0;
}

bool IOManager::delEvent(int fd, Event event) {
    return true;
}

bool IOManager::cancelEvent(int fd, Event event) {
    return true;
}

bool IOManager::cancelAll(int fd) {
    return true;
}

// static 

IOManager::ptr IOManager::GetThis() {
    return std::dynamic_pointer_cast<IOManager>(Scheduler::GetThis());
    // return (IOManager::ptr)((IOManager *)&*(Scheduler::GetThis()));
}

// protected

void IOManager::tickle() {
}

bool IOManager::stopping() {
    return true;
}

void IOManager::idle() {
}

bool IOManager::stopping(uint64_t & time_out) {
    return true;
}

void IOManager::contextResize(size_t size) {
    m_fdCtxs.resize(size);

    for (size_t i = 0; i < size; i++) {
        if (!m_fdCtxs[i]) {
            m_fdCtxs[i].reset(new FdCtx);
            // 同一个 fd 可能有多个 event (对 socket 操作即对 socket fd 操作)
            m_fdCtxs[i]->fd = i;
        }
    }
}

}
