#ifndef     __ZHOU_IOMANAGER_H__
#define     __ZHOU_IOMANAGER_H__

#include "scheduler.h"
#include "zhou/thread/rwlock.h"
#include "zhou/timer/timer_manager.h"
#include <memory>

namespace zhou { // __ZHOU_IOMANAGER_H__

class IOManager : public Scheduler, public TimerManager {
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    enum Event {
        NONE = 0x0,
        READ = 0x1,
        WRITE = 0x4,
    };

public:
    IOManager(size_t thread_count = 1, bool use_caller = true, const std::string & name = "");
    ~IOManager();

    // 0: success; -1: error
    int addEvent(int fd, Event event, std::function<void()> callback = nullptr);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);
    bool cancelAll(int fd);

public:
    static IOManager * GetThis();
    // void stop();

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;

    // bool stopping(uint64_t & time_out);
    void contextResize(size_t size);

    // 当有新的定时器插入到定时器的首部，执行该函数
    void onTimerInsertedAtFront() override;

private:
    // 在 epoll 中增加事件是根据一个个 fd 来判断的
    struct FdCtx {
        typedef Mutex MutexType;
        typedef std::shared_ptr<FdCtx> ptr;

        struct EventCtx {
            Scheduler::ptr scheduler;              // 在哪个 Scheduler 上执行，被哪个 Scheduler 调度处理
            Fiber::ptr fiber;                   // IO 事件的协程
            std::function<void()> callback;     // IO 事件的回调
        };

        // 获取事件上下文
        EventCtx & getContext(Event event);
        // 重置事件上下文
        void resetContext(EventCtx & ctx);
        // 触发事件
        void triggerEvent(Event event);

        EventCtx read_ctx;
        EventCtx write_ctx;
        int fd = 0;                 // 当前
        Event events;           // 当前 FdCtx 代表的 IO 事件
        MutexType mutex;
    };

private:
    int m_epfd = 0;                                     // epoll 文件句柄
    int m_tickleFds[2];                                 // pipe 文件句柄
    std::atomic<size_t> m_pendingEventCount = {0};      // 当前等待执行的事件数量
    RWMutexType m_mutex;
    std::vector<FdCtx::ptr> m_fdCtxs;                   // socket IO 事件 上下文的容器

};

}   // ! __ZHOU_IOMANAGER_H__

#endif      // ! __ZHOU_IOMANAGER_H__

