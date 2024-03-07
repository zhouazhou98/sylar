#include "zhou/zhou.h"

zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void fun1() {
    ZHOU_INFO(g_logger) << "thread name = " << zhou::Thread::GetName() 
                << ", this->name = " << zhou::Thread::GetThis()->getName()
                << ", id = " << syscall(SYS_gettid)
                << ", this->id = " << zhou::Thread::GetThis()->getId();
    return ;
}

void fun2() {

    return ;
}


int main() {
    g_logger->addAppender(zhou::LogAppender::ptr(new zhou::StdoutLogAppender));
    ZHOU_INFO(g_logger) << "thread test begin";
    std::vector<zhou::Thread::ptr> threads;
    for (int i = 0; i < 5; i++) {
        zhou::Thread::ptr thr(
                        new zhou::Thread(&fun1, "thread_" + std::to_string(i))
                );
        threads.push_back(thr);
    }
    for (int i = 0; i < 5; i++) {
        threads[i]->join();
    }
    ZHOU_INFO(g_logger) << "thread test end";
    return 0;    
}
