
#include "zhou/zhou.h"


static zhou::Logger::ptr logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void test() {
    ZHOU_ERROR(logger) << "\n" << zhou::BacktraceToStriing(100, 2, "\t");
}

int main() {
    test();

    return 0;
}
