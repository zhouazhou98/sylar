#include <iostream>
#include "zhou/log/log.h"
#include <pthread.h>

#define ZHOU_LOG(logger, level)    \
    if (logger->getLevel() <= level)    \
        zhou::LogEventWrap(   logger, \
                        level,  \
                        zhou::LogEvent::ptr(new zhou::LogEvent(__FILE__, __LINE__, 0, 0, 0, time(0)) )   \
            ).getSS()

#define ZHOU_TRACE(logger)  ZHOU_LOG(logger, zhou::LogLevel::TRACE)
#define ZHOU_DEBUG(logger)  ZHOU_LOG(logger, zhou::LogLevel::DEBUG)
#define ZHOU_INFO(logger)   ZHOU_LOG(logger, zhou::LogLevel::INFO)
#define ZHOU_WARN(logger)   ZHOU_LOG(logger, zhou::LogLevel::WARN)
#define ZHOU_ERROR(logger)  ZHOU_LOG(logger, zhou::LogLevel::ERROR)
#define ZHOU_FATAL(logger)  ZHOU_LOG(logger, zhou::LogLevel::FATAL)

#include <sys/syscall.h>
#include <unistd.h>

int main() {
    zhou::Logger::ptr logger(new zhou::Logger);
    logger->addAppender(zhou::LogAppender::ptr(new zhou::StdoutLogAppender));

    // zhou::LogEvent::ptr event(new zhou::LogEvent(__FILE__, __LINE__, 0, 0, 0, time(0)/*, "hello world!" */));
    zhou::LogEvent::ptr event(new zhou::LogEvent(__FILE__, __LINE__, 0, syscall(SYS_gettid), 0, time(0)/*, "hello world!" */));
            printf("pid 父进程所属线程 ID 为 %ld\n",pthread_self());


    event->getSS() << "hello world!hhhhh";
    // std::cout << event->getContent() << std::endl;
    logger->log(zhou::LogLevel::INFO, event);

    ZHOU_LOG(logger, zhou::LogLevel::INFO) << "ok";
    ZHOU_ERROR(logger) << "ok";

    std::cout << "hello world!\n";
    return 0;
}