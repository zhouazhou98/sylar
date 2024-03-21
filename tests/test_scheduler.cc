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
    zhou::Scheduler::ptr sc(new zhou::Scheduler(1, true, "hello"));
    sc->start();
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
