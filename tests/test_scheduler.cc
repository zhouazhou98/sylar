#include "zhou/zhou.h"

zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

int main() {
    zhou::Scheduler::ptr sc(new zhou::Scheduler(2, true, "hello"));
    sc->start();
    sc->stop();
    return 0;
}
