#include "zhou/zhou.h"

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void test_hook_sleep() {
    zhou::IOManager::ptr iom(new zhou::IOManager(2, false));
    iom->start();
    sleep(1);
    iom->stop();
}


int main() {
    test_hook_sleep();
    return 0;
}
