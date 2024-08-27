#include "zhou/zhou.h"

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

zhou::Timer::ptr s_timer;

void test_timer() {
    zhou::IOManager::ptr iom(new zhou::IOManager(2, false));
    iom->start();
    s_timer = iom->addTimer(1000, [](){
        static int i = 0;
        ZHOU_INFO(g_logger) << "hello timer i = " << i;
        if(++i == 3) {
            // s_timer->refresh();
            // s_timer->reset(2000, true);
            s_timer->cancel();
        }
    }, true);

    zhou::Timer::ptr timer = iom->addTimer(1000, [](){
            ZHOU_INFO(g_logger) << "hello";
        },
        false
    );
    iom->stop();
}

int main(int argc, char** argv) {
    test_timer();
    return 0;
}