#ifndef __ZHOU_SCHEDULER_H__
#define __ZHOU_SCHEDULER_H__

#include <memory>
#include "fiber.h"
#include "zhou/thread/lock.h"
#include "zhou/thread/thread.h"
#include <vector>
#include <list>

namespace zhou {

class Scheduler : public std::enable_shared_from_this<Scheduler> {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    // threads: 线程池中分配多少个线程
    // use_caller: 有时候需要将一个线程放到协程调度器管理
    //              我们在哪个线程执行了协程调度器的构造函数时，如果该参数设置为 true 则将该线程也纳入到 Scheduler 中进行管理
    // name: 调度器名称
    Scheduler(int threads = 1, bool use_caller = true, const std::string & name = "");
    virtual ~Scheduler();

    const std::string & getName() { return m_name; }


    void setThis();
    // 获得当前调度器
    static Scheduler::ptr GetThis();
    // 调度器的主协程
    //      注意区分与线程的主协程的区别
    static Fiber::ptr GetMainFiber();

    void start();
    void stop();

    template <class FiberOrCallback>
    void schedule(FiberOrCallback fc, int thread_id = -1) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread_id);
        }
        if (need_tickle) {
            // 如果 m_fibers 为空 则执行 tickle
            //  B 站弹幕： 陈硕书上说只要有就需要 notify (tickle) ， 而不是等到空了再 tickle
            tickle();
        }
    }

    template <class FiberOrCallbackIterator>
    void scheduleIters(FiberOrCallbackIterator begin, FiberOrCallbackIterator end) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while (begin != end) {
                // 这里 &*begin 是指针， 在 scheduleNoLock 中会在堆上创建一个 FiberAndFunc 类型， 其调用的是含有二级指针的初始化函数， 即会 swap 掉这里的 begin
                need_tickle = scheduleNoLock(&*begin++) || need_tickle;
                // 到了这里， 这个 begin 已经被 swap 掉了， 不能再继续对 begin 进行操作
                //      而对于这个函数来说， 作为实参传进来的 begin 在调用该函数的作用域中也不能用了
            }
        }
        if (need_tickle) {
            // 如果 m_fibers 为空 则执行 tickle
            //  B 站弹幕： 陈硕书上说只要有就需要 notify (tickle) ， 而不是等到空了再 tickle
            tickle();
        }
    }

protected:
    virtual void tickle();
    virtual bool stopping();
    void run();
    virtual void idle();

private:
    template <class FiberOrCallback>
    bool scheduleNoLock(FiberOrCallback fc, int thread_id) {
        bool need_tickle = m_fibers.empty();
        FiberAndFunc::ptr fc_p(new FiberAndFunc(fc, thread_id) );
        if (fc_p->fiber || fc_p->callback) {
            m_fibers.push_back(fc_p);
        }

        // 返回 need_tickle 表示 是否需要调度
        //      是不是应该把 need_tickle 初始化放到这里来
        return need_tickle;
    }


private:
    // struct 默认 public ，因此这里命名不给 m_ 前缀
    struct FiberAndFunc {
        typedef std::shared_ptr<FiberAndFunc> ptr;

        Fiber::ptr fiber;
        std::function<void()> callback;
        int thread_id;     // 获取当前这个 协程 或 函数 在哪个线程上执行

        FiberAndFunc(Fiber::ptr arg_fiber, int arg_thread_id)
            : fiber(arg_fiber), thread_id(arg_thread_id) {
        }
        // 调用该函数的 arg_fiber_p 会失去对原本指向的协程的控制权，转而置为 nullptr
        //      目的： 直接让栈上或堆上指向协程的智能指针清空， 防止协程引用计数永远不为 0 导致不能正常析构
        FiberAndFunc(Fiber::ptr * arg_fiber_p, int arg_thread_id)
            : thread_id(arg_thread_id) {
            fiber.swap(*arg_fiber_p);
        } 
        FiberAndFunc(std::function<void()> arg_callback, int arg_thread_id)
            : callback(arg_callback), thread_id(arg_thread_id) {
        }
        FiberAndFunc(std::function<void()> * arg_callback, int arg_thread_id)
            : thread_id(arg_thread_id) {
            callback.swap(*arg_callback);
        }

        // 再给一个默认构造函数
        //  STL 中的内容： STL 中在分配对象时一定需要一个默认构造函数， 否则分配出来的对象无法初始化
        FiberAndFunc() : thread_id(-1) {

        }

        void reset() {
            fiber = nullptr;
            callback = nullptr;
            thread_id = -1;
        }
    };

private:
    MutexType m_mutex;
    std::vector<Thread::ptr> m_threads; // 线程池
    std::string m_name;

    // 调度器的主协程： 当使用的线程是一个新的线程时， 新的线程的主协程并不会参与到我们的协程调度中去， 因此我们要专门做一个新的协程去做 schdule
    Fiber::ptr m_rootFiber;     // 只在创建本调度器的线程也需要被调度时才会有值
    // ucontext_t m_exit_ctx;
    ucontext_t m_root_fiber_ctx;    // 保存在 m_rootFiber 协程栈上 协程相关的上下文切换信息

    // m_fibers 并不见得都是 fiber 对象， 也可能是一个函数
    //      总之，一个协程也不过是一段代码执行逻辑
    // B 站弹幕： 这个是有栈非对称协程
    std::list<FiberAndFunc::ptr> m_fibers;  // 用来保存即将要执行或计划要执行的 协程 / 函数



protected:
    std::vector<int> m_threadIds;       // 保存线程 ID： 方便后续指定线程运行
    size_t m_threadCount = 0;           // 线程总数
    size_t m_activeThreadCount = 0;     // 活跃线程的数量
    size_t m_idleThreadCount = 0;       // 空闲线程的数量
    bool m_stopping = true;             // 执行状态： 是否停止
    bool m_autoStop = false;            // 是否自动停止
    int m_rootThreadId = 0;             // 即 use_caller 的 ID
};

}


#endif // ! __ZHOU_SCHEDULER_H__
