
#include "iomanager.h"
#include "zhou/utils/util.h"

namespace zhou {

// 当有新的定时器插入到定时器的首部，执行该函数
void IOManager::onTimerInsertedAtFront() {
    tickle();
}


bool IOManager::stopping(uint64_t & timeout) {
    timeout = getNextTimer();
    return timeout == ~0ull 
        && m_pendingEventCount == 0
        && Scheduler::stopping();
}
}
