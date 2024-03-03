#include <iostream>
#include "zhou/log/log.h"
int main() {
    zhou::Logger::ptr logger(new zhou::Logger);
    logger->addAppender(zhou::LogAppender::ptr(new zhou::StdoutLogAppender));

    std::cout << "hello world!\n";
    return 0;
}