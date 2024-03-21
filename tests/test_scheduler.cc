#include "zhou/zhou.h"
#include "zhou/fiber/fiber.h"
#include <iostream>

zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

static zhou::Mutex mutex;

void test() {
    zhou::Mutex::Lock lock(mutex);
    static int test = 0;
    test++;
    std::cout << "thread_id: " << syscall(SYS_gettid) << "\ttest: " << test << std::endl;

}

void test2() {
    zhou::Mutex::Lock lock(mutex);
    static int test2 = 0;
    test2++;
    std::cout << "thread_id: " << syscall(SYS_gettid) << "\ttest2: " << test2 << std::endl;

}


int main() {
    zhou::Scheduler::ptr sc(new zhou::Scheduler(10, true, "hello"));
    sc->start();
    // 如果不调用 scheduler 则永远不会调用 swapIn 函数
    //      如果调用一次 scheduler 则调用三次 swapIn 函数
    //  1. m_rootFiber              --> while 循环
    //  2. idleFiber                --> while 循环过程中会调用到 
    //  3. schedule test function   --> while 循环主逻辑
    // 可以看到有三次上下文都是 while 循环

    sc->schedule([](){ zhou::Mutex::Lock lock(mutex); std::cout << "hello world!" << std::endl; });
    sc->schedule(&test);
    sc->schedule(&test);
    sc->schedule(&test);
    sc->schedule(&test);
    sc->schedule(&test);
    sc->schedule(&test);
    sc->schedule(&test);
    sc->schedule(&test);
    sc->schedule(&test);
    
    sc->schedule(&test2);
    sc->schedule(&test2);
    sc->schedule(&test2);
    sc->schedule(&test2);
    sc->schedule(&test2);
    sc->schedule(&test2);
    sc->schedule(&test2);
    sc->schedule(&test2);
    sc->schedule(&test2);
    sc->stop();
    return 0;
}
