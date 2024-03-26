#include "zhou/zhou.h"

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void test_hook_sleep() {
    sleep(1);
}


int main() {
    test_hook_sleep();
    return 0;
}
