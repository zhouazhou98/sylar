#include <iostream>
#include "zhou/log/log.h"
int main() {
    zhou::Logger::ptr logger(new zhou::Logger);
    logger->addAppender(zhou::LogAppender::ptr(new zhou::StdoutLogAppender));

    zhou::LogEvent::ptr event(new zhou::LogEvent(__FILE__, __LINE__, 0, 0, 0, time(0), "hello world!"));

    // std::cout << event->getContent() << std::endl;
    logger->log(zhou::LogLevel::INFO, event);

    //std::cout << "hello world!\n";
    return 0;
}