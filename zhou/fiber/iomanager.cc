
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
// static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

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
}

IOManager::~IOManager() {
    // stop();
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);
    close(m_epfd);
}

// 0: success; -1: error
int IOManager::addEvent(int fd, Event event, std::function<void()> callback) {
// 1. 从容器 m_fdCtxs 中取出 fd 对应的智能指针
//  1.1 m_fdCtxs[fd] 指向的 FdCtx 只是初始化了，没有任何内容
//  1.2 m_fdCtxs[fd] 指向的 FdCtx 已经在初始化后修改过了，比如对该 fd 添加了事件等
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

// 2. 为 fd_ctx 添加 event 事件，并将其添加到 m_epfd 中去
    FdCtx::MutexType::Lock lock(fd_ctx->mutex);
    // 2.1 如果 fd_ctx 中已经监听了 event 事件， 则这次添加是有问题的
    //  这里是针对 1.2 中已经对该 fd 添加过事件，这里又重新添加该事件的处理
    if (fd_ctx->events & event) {
        ZHOU_ERROR(g_logger) << "addEvent assert fd=" << fd
            << " event=" << event
            << " fd_ctx.event=" << fd_ctx->events;
        ZHOU_ASSERT(!(fd_ctx->events & event));
    }

    // 2.2 将 fd, event 添加到 m_epfd 中
    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event ep_event;
    ep_event.events = EPOLLET | fd_ctx->events | event;
    ep_event.data.ptr = fd_ctx.get();

    int rt = epoll_ctl(m_epfd, op, fd, &ep_event);
    if (rt) {
        ZHOU_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << "," << fd << "," << ep_event.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return -1;
    }

    // 2.3 设置事件触发回调函数
    ++m_pendingEventCount;
    fd_ctx->events = (Event)(fd_ctx->events | event | EPOLLET);
    FdCtx::EventCtx & event_ctx = fd_ctx->getContext(event);
    ZHOU_ASSERT(!event_ctx.scheduler
                && !event_ctx.fiber
                && !event_ctx.callback);
    
    event_ctx.scheduler = Scheduler::GetThis();
    if (callback) {
        event_ctx.callback.swap(callback);
    } else {
        event_ctx.fiber = Fiber::GetThis();
        ZHOU_ASSERT2(event_ctx.fiber->getState() == Fiber::EXEC,
                        "state = " << event_ctx.fiber->getState());
    }
    return 0;
}

bool IOManager::delEvent(int fd, Event event) {
// 1. 从 m_fdCtxs 中取出 fd_ctx
    FdCtx::ptr fd_ctx;
    {
    RWMutexType::ReadLock lock(m_mutex);
        if ((int)m_fdCtxs.size() <= fd) {
            return false;
        }
        fd_ctx = m_fdCtxs[fd];
    }
// 2. 将相应的文件句柄对应事件从 m_epfd 上摘下来
//  2.1 只对 fd 监听了 event 事件， 则删除
//  2.2 对 fd 还监听了其它 event 事件， 则修改
    FdCtx::MutexType::Lock lock2(fd_ctx->mutex);
    // 该 fd 对应的事件应该在监听事件中
    if (!(fd_ctx->events & event)) {
        return false;
    }

    Event new_event = (Event)(fd_ctx->events & ~event);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event ep_event;
    ep_event.events = EPOLLET | new_event;
    ep_event.data.ptr = fd_ctx.get();

    int rt = epoll_ctl(m_epfd, op, fd, &ep_event);
    if(rt) {
        ZHOU_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << "," << fd << "," << ep_event.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

// 3. 重置 fd 对应需要删除的 event 事件 上下文
    --m_pendingEventCount;
    fd_ctx->events = new_event;
    FdCtx::EventCtx & event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);

    return true;
}

bool IOManager::cancelEvent(int fd, Event event) {
// 1. 从 m_fdCtxs 中取出 fd_ctx
    FdCtx::ptr fd_ctx;
    {
    RWMutexType::ReadLock lock(m_mutex);
        if ((int)m_fdCtxs.size() <= fd) {
            return false;
        }
        fd_ctx = m_fdCtxs[fd];
    }

// 2. 将相应的文件句柄对应事件从 m_epfd 上摘下来
//  2.1 只对 fd 监听了 event 事件， 则删除
//  2.2 对 fd 还监听了其它 event 事件， 则修改
    FdCtx::MutexType::Lock lock2(fd_ctx->mutex);
    // 该 fd 对应的事件应该在监听事件中
    if (!(fd_ctx->events & event)) {
        return false;
    }

    Event new_event = (Event)(fd_ctx->events & ~event);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event ep_event;
    ep_event.events = EPOLLET | new_event;
    ep_event.data.ptr = fd_ctx.get();

    int rt = epoll_ctl(m_epfd, op, fd, &ep_event);
    if(rt) {
        ZHOU_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << "," << fd << "," << ep_event.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

// 3. 触发事件
    
    fd_ctx->triggerEvent(event);
    --m_pendingEventCount;
    return true;
}

bool IOManager::cancelAll(int fd) {
// 1. 从 m_fdCtxs 中取出 fd_ctx
    FdCtx::ptr fd_ctx;
    {
    RWMutexType::ReadLock lock(m_mutex);
        if ((int)m_fdCtxs.size() <= fd) {
            return false;
        }
        fd_ctx = m_fdCtxs[fd];
    }
// 2. 将相应的文件句柄对应事件从 m_epfd 上摘下来
//  2.1 只对 fd 监听了 event 事件， 则删除
//  2.2 对 fd 还监听了其它 event 事件， 则修改
    FdCtx::MutexType::Lock lock2(fd_ctx->mutex);
    // 该 fd 对应的事件应该在监听事件中
    if (!fd_ctx->events) {
        return false;
    }

    int op = EPOLL_CTL_MOD;
    epoll_event ep_event;
    ep_event.events = 0;
    ep_event.data.ptr = fd_ctx.get();

    int rt = epoll_ctl(m_epfd, op, fd, &ep_event);
    if(rt) {
        ZHOU_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << "," << fd << "," << ep_event.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

// 3. 删除前先触发事件
    if(fd_ctx->events & READ) {
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
    }
    if(fd_ctx->events & WRITE) {
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;
    }

    ZHOU_ASSERT(fd_ctx->events == 0);

    return true;
}

// static 

IOManager * IOManager::GetThis() {
    return (IOManager *)Scheduler::GetThis().get();
    // return std::dynamic_pointer_cast<IOManager>(Scheduler::GetThis());
    // return (IOManager::ptr)( (IOManager *)Scheduler::GetThis().get() );
}

// void IOManager::stop() {
//     Scheduler::stop();
//     for (size_t i = 0; i < m_fdCtxs.size(); i++) {
//         if (m_fdCtxs[i]->read_ctx.scheduler) {
//             m_fdCtxs[i]->read_ctx.scheduler.reset();
//         } 
//         if (m_fdCtxs[i]->write_ctx.scheduler) {
//             m_fdCtxs[i]->write_ctx.scheduler.reset();
//         }
//     }
// 
// 
// }

// protected

void IOManager::tickle() {
    if (!m_idleThreadCount) {
        return;
    }
    int rt = write(m_tickleFds[1], "T", 1);
    ZHOU_ASSERT(rt == 1);       // rt 为实际写入的长度
}

bool IOManager::stopping() {
    uint64_t timeout = 0;
    return stopping(timeout);
}

// 核心的调度操作 （与 Scheduler::run 一起）
//      按理来说 epoll_wait 会阻塞， 但是我们这里为 epoll 增加了一个 pipe 管道
//      当我们要让线程继续执行时就使用 tickle 发起写事件， epoll 中对管道监听 读 事件
void IOManager::idle() {
    epoll_event * events = new epoll_event[64]();
    // 自定义 析构函数
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event * ptr) {
        delete [] ptr;
    });

    while (true) {
        uint64_t next_timeout = 0;
    // 1. 检查是否停止
        // stopping 中获取下一个回调函数调用距离现在的时间
        if (stopping(next_timeout)) {
            ZHOU_INFO(g_logger) << "name = " << getName() << "idle stopping exit";
            return;
        }

    // 2. epoll_wait 超时时间设置， 陷入 epoll_wait 等待事件
        int rt = 0;
        
        do {
            static const int MAX_TIMEOUT = 3000;
            if (next_timeout != ~0ull) {
                next_timeout = (int)next_timeout > MAX_TIMEOUT 
                                    ? MAX_TIMEOUT : next_timeout;
            } else {
                next_timeout = MAX_TIMEOUT;
            }

            // rt > 0 : 准备好的事件
            rt = epoll_wait(m_epfd, events, 64, (int)next_timeout);
            if (rt < 0 && errno == EINTR) {
            } else {
                break;
            }
        } while (true);

    // 3. schedule 所有的回调函数
        std::vector<std::function<void()>> callbacks;
        listExpiredCallback(callbacks);
        if (!callbacks.empty()) {
            for (auto i : callbacks) {
                schedule(i);
            }
            // scheduleIters(callbacks.begin(), callbacks.end());
            callbacks.clear();
        }

    // 4. 调度执行: pipe 管道; socket IO 事件
        for (int i = 0; i < rt; i++) {
            epoll_event & event = events[i];

        // 3.1 pipe 管道读事件
            if (event.data.fd == m_tickleFds[0]) {
                uint8_t dummy;
                while (read(m_tickleFds[0], &dummy, 1) == 1);
                continue;
            }

        // 3.2 IO 事件
            // 3.2.1 检查触发事件
            FdCtx * fd_ctx = (FdCtx *)event.data.ptr;
            FdCtx::MutexType::Lock lock(fd_ctx->mutex);
            if (event.events & (EPOLLERR | EPOLLHUP)) {
                event.events |= EPOLLIN | EPOLLOUT;
            }
            int real_events = NONE;
            if (event.events & EPOLLIN) {
                real_events |= READ;
            }
            if (event.events & EPOLLOUT) {
                real_events |= WRITE;
            }

            // 3.2.2 如果没有触发的事件则执行下一个
            if ( (fd_ctx->events & real_events) == NONE ) {
                continue;
            }

            // 3.2.3 设置剩下的事件 并继续将其加入到 epoll 中
            //      修改 fd 对应的事件， 或 删除 fd 对应的事件
            int left_events = (fd_ctx->events & ~real_events);
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;

            int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if(rt2) {
                ZHOU_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
                    << op << "," << fd_ctx->fd << "," << event.events << "):"
                    << rt2 << " (" << errno << ") (" << strerror(errno) << ")";
                continue;
            }

            // 3.2.4 触发对应的事件
            if(real_events & READ) {
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }
            if(real_events & WRITE) {
                fd_ctx->triggerEvent(WRITE);
                --m_pendingEventCount;
            }
        }

        // 对应于 Scheduler::MainFunc 在调用 callback() 后的处理
        //  Fiber::ptr cur = Fiber::GetThis();
        //  auto raw_ptr = cur.get();
        //  cur.reset();

        //  raw_ptr->swapOut();

        Fiber::GetThis().get()->swapOut();

    }

}

// bool IOManager::stopping(uint64_t & time_out) {
//     return true;
// }

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
