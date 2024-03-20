#include "zhou/zhou.h"
#include "zhou/fiber/fiber.h"
#include <iostream>

zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void test() {
    static int test = 0;
    test++;
    std::cout << "test: " << test << std::endl;

}


int main() {
    zhou::Scheduler::ptr sc(new zhou::Scheduler(1, true, "hello"));
    sc->start();
    sc->schedule([](){ std::cout << "hello world!" << std::endl; });
    sc->schedule(&test);
    sc->stop();
    return 0;
}
