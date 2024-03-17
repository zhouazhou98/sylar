
#include "zhou/zhou.h"


static zhou::Logger::ptr logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void test() {
    ZHOU_ERROR(logger) << "\n" << zhou::BacktraceToString(100, 2, "\t");
    ZHOU_ASSERT2(1==0, "hello world!");
    ZHOU_ASSERT(1==0);
}

int main() {
    test();

    return 0;
}
