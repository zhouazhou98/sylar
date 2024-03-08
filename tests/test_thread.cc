#include "zhou/zhou.h"
#include <unistd.h>

zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void fun1() {
    ZHOU_INFO(g_logger) << "thread name = " << zhou::Thread::GetName() 
                << ", this->name = " << zhou::Thread::GetThis()->getName()
                << ", id = " << syscall(SYS_gettid)
                << ", this->id = " << zhou::Thread::GetThis()->getId();
    
    // ps aux | grep test_thread : 命令可以查看到 test_thread 测试程序进程号
    // top -H -p {测试程序进程号}  ： 命令可以查看到由 test_thread 测试程序产生的线程
    // 注意： 这里的 sleep 就是为了线程不退出，便于用命令行查看
    sleep(60);
    
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
