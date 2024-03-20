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


int main() {
    zhou::Scheduler::ptr sc(new zhou::Scheduler(3, false, "hello"));
    sc->start();
    sc->schedule([](){ zhou::Mutex::Lock lock(mutex); std::cout << "hello world!" << std::endl; });
    sc->schedule(&test);
    sc->schedule(&test);
    sc->schedule(&test);
    sc->stop();
    return 0;
}
