
#include "iomanager.h"

namespace zhou {

// 当有新的定时器插入到定时器的首部，执行该函数
void IOManager::onTimerInsertedAtFront() {
    tickle();
}
}
