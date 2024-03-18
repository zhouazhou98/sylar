


#include "zhou/zhou.h"

zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void test() {
    ZHOU_INFO(g_logger) << "run in fiber begin";
    // zhou::Fiber::GetThis()->YeildToHold();
    zhou::Fiber::GetThis()->swapOut();
    ZHOU_INFO(g_logger) << "run in fiber end";

}

void test_fiber() {
    ZHOU_INFO(g_logger) << "test fiber fucntion begin";
    zhou::Fiber::ptr fiber(new zhou::Fiber(&test));
    ZHOU_INFO(g_logger) << "before swap in fiber";
    fiber->swapIn();
    ZHOU_INFO(g_logger) << "after swap in fiber";
    fiber->swapIn();
    ZHOU_INFO(g_logger) << "test fiber function end";
}

int main() {
    zhou::Thread::SetName("main");

    ZHOU_INFO(g_logger) << "main begin";
    
    std::vector<zhou::Thread::ptr> threads;
    for (int i = 0; i < 3; i++) {
        threads.push_back(
            zhou::Thread::ptr(new zhou::Thread(&test_fiber, "thread_" + std::to_string(i)))
        );
    }

    for (int i = 0; i < 3; i++) {
        threads[i]->join();
    }

    ZHOU_INFO(g_logger) << "main end";

    return 0;
}
