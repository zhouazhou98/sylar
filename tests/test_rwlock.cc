#include "zhou/zhou.h"
#include <unistd.h>

zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");


int count = 0;
zhou::RWMutex s_rwlock;

void fun1() {
    ZHOU_INFO(g_logger) << "thread name = " << zhou::Thread::GetName() 
                << ", this->name = " << zhou::Thread::GetThis()->getName()
                << ", id = " << syscall(SYS_gettid)
                << ", this->id = " << zhou::Thread::GetThis()->getId();
    
    for (int i = 0; i < 100000000; i ++) {
        ++count;
    }
    return ;
}

void fun2() {
    ZHOU_INFO(g_logger) << "thread name = " << zhou::Thread::GetName() 
                << ", this->name = " << zhou::Thread::GetThis()->getName()
                << ", id = " << syscall(SYS_gettid)
                << ", this->id = " << zhou::Thread::GetThis()->getId();
    
    for (int i = 0; i < 100000000; i ++) {
        zhou::RWMutex::WriteLock lock(s_rwlock);
        ++count;
    }
    return ;
}


int main() {
    g_logger->addAppender(zhou::LogAppender::ptr(new zhou::StdoutLogAppender));
    ZHOU_INFO(g_logger) << "thread test begin";
    std::vector<zhou::Thread::ptr> threads;


    for (int i = 0; i < 5; i++) {
        zhou::Thread::ptr thr(
                        new zhou::Thread(&fun1, "thread_lock_" + std::to_string(i))
                );
        threads.push_back(thr);
    }
    for (int i = 0; i < 5; i++) {
        threads[i]->join();
    }
    ZHOU_INFO(g_logger) << "thread test end";
    ZHOU_INFO(g_logger) << count;

// ----------------- 增加读写锁 ----------------------------------
    std::vector<zhou::Thread::ptr> rw_threads;
    count = 0;
    for (int i = 0; i < 5; i++) {
        zhou::Thread::ptr thr(
                        new zhou::Thread(&fun2, "thread_rwlock_" + std::to_string(i))
                );
        rw_threads.push_back(thr);
    }
    for (int i = 0; i < 5; i++) {
        rw_threads[i]->join();
    }
    ZHOU_INFO(g_logger) << "thread test end";
    ZHOU_INFO(g_logger) << count;

    return 0;    
}
