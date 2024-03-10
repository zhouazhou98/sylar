#include "zhou/zhou.h"
#include <unistd.h>

zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void fun1() {
    while(1)
        ZHOU_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    return ;
}
void fun2() {
    while(1)
        ZHOU_INFO(g_logger) << "====================================================";

    return ;
}


int main() {
    g_logger->addAppender(zhou::LogAppender::ptr(new zhou::FileLogAppender("log/log.txt")));
    // ZHOU_INFO(g_logger) << "thread test begin";
    std::vector<zhou::Thread::ptr> threads;


    for (int i = 0; i < 4; i++) {
        zhou::Thread::ptr thr(
                        new zhou::Thread(&fun1, "thread_" + std::to_string(i * 2))
                );
        zhou::Thread::ptr thr2(
                        new zhou::Thread(&fun2, "thread_" + std::to_string(i * 2 + 1))
                );
        threads.push_back(thr);
        threads.push_back(thr2);
    }
    for (size_t i = 0; i < threads.size(); i++) {
        threads[i]->join();
    }
    // ZHOU_INFO(g_logger) << "thread test end";
    return 0;    
}
