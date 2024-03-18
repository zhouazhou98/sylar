

#include "zhou/zhou.h"

zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void test() {
    ZHOU_INFO(g_logger) << "run in fiber begin";
    zhou::Fiber::GetThis()->YeildToHold();
    // zhou::Fiber::GetThis()->swapOut();
    ZHOU_INFO(g_logger) << "run in fiber end";

}

int main() {
    ZHOU_INFO(g_logger) << "main fucntion begin";
    zhou::Fiber::ptr main_fiber = zhou::Fiber::GetThis();
    zhou::Fiber::ptr fiber(new zhou::Fiber(&test));
    ZHOU_INFO(g_logger) << "before swap in fiber";
    fiber->swapIn();
    ZHOU_INFO(g_logger) << "after swap in fiber";
    fiber->swapIn();
    ZHOU_INFO(g_logger) << "fiber end";

    return 0;
}
