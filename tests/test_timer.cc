#include "zhou/zhou.h"

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

zhou::Timer::ptr s_timer;
zhou::Timer::ptr timer;

zhou::RWMutex g_mutex;
    

void test_timer(zhou::IOManager::ptr iom) {
    iom->start();
    s_timer = iom->addTimer(1000, [](){
        static int i = 0;
        {
            zhou::RWMutex::WriteLock lock(g_mutex);
        ZHOU_INFO(g_logger) << "hello timer i = " << i;
            i = i + 1;
            if(i == 30) {
                // s_timer->refresh();
                // s_timer->reset(2000, true);
                ZHOU_INFO(g_logger) << "cancel: " <<

                s_timer->cancel();
            }
        }
    }, true);

    timer = iom->addTimer(1000, [](){
            ZHOU_INFO(g_logger) << "cancel: " <<
            timer->cancel();
        },
       true 
    );
    iom->stop();
}

int main(int argc, char** argv) {
    zhou::IOManager::ptr iom(new zhou::IOManager(2, false));
    test_timer(iom);
    return 0;
}