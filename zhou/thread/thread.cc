#include "thread.h"

namespace zhou {

static zhou::Logger::ptr g_logger = SingleLoggerManager::GetInstance()->getLogger("system");

static thread_local Thread * t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOWN";

Thread * Thread::GetThis() {
    return t_thread;
}
std::string& Thread::GetName() {  // 给日志使用
    return t_thread_name;
}
void Thread::SetName(const std::string & name) {
    if (t_thread)
        t_thread->m_name = name;

    t_thread_name = name;
}

void * Thread::run(void * arg) {
    Thread * thread = (Thread *)arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = syscall(SYS_gettid);
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
    
    // 为了防止当函数中存在智能指针时，防止它的引用，为出现在日志不会被释放掉
    std::function<void()> callback;
    callback.swap(thread->m_callback);

    callback();
    return 0;
}


Thread::Thread(std::function<void()> callback, const std::string & name)
        : m_callback(callback), m_name(name) {
    if(name.empty()) {
        m_name = "UNKNOWN";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt) {
        ZHOU_ERROR(g_logger) << "pthread_create failed, rt = " << rt << ", name = " << name;
        throw std::logic_error("pthread_create error!");
    }
}
Thread::~Thread() {
    if (m_thread) {
        // 线程析构
        // pthread_destroy
        pthread_detach(m_thread);
    }
}


void Thread::join() {
    if (m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if (rt) {
            ZHOU_ERROR(g_logger) << "pthread_join failed, rt = " << rt << ", name = " << m_name;
            throw std::logic_error("pthread_join error!");
        }
        // TODO: 为什么这里 join 等待 m_thread 线程运行结束后需要将 m_thread 置空
        m_thread = 0;
    }
}
}
