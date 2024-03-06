#include <iostream>
#include "zhou/log/log.h"
#include "zhou/utils/util.h"
#include "zhou/utils/macro.h"
#include "zhou/log/log_manager.h"


int main() {
    zhou::Logger::ptr logger(new zhou::Logger);
    // logger->addAppender(zhou::LogAppender::ptr(new zhou::StdoutLogAppender));

    // zhou::LogEvent::ptr event(new zhou::LogEvent(__FILE__, __LINE__, 0, syscall(SYS_gettid), 0, time(0)/*, "hello world!" */));
    // std::cout << event->getContent() << std::endl;
    // logger->log(zhou::LogLevel::INFO, event);


    zhou::LogAppender::ptr file_appender = zhou::LogAppender::ptr(new zhou::FileLogAppender("log/log.txt"));

    file_appender->setFormatter(
                    zhou::LogFormatter::ptr(
                        new zhou::LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n")
            ));
    file_appender->setLevel(zhou::LogLevel::ERROR);
    logger->addAppender(file_appender);




    ZHOU_LOG(logger, zhou::LogLevel::INFO) << "ok";
    ZHOU_ERROR(logger) << "ok";

    ZHOU_LOG_FMT(logger, zhou::LogLevel::INFO, "hello%s", "world");
    ZHOU_DEBUG_FMT(logger, "hello%s", "world");
    ZHOU_INFO_FMT(logger, "hello%s", "world");
    ZHOU_INFO_FMT(logger, "hello");

    ZHOU_DEBUG(logger) << "hello";

    for (int i = 0; i < 1000000; i++) {
        ZHOU_ERROR(logger) << "test\t" << i << "\t";
    }


    auto l = zhou::SingleLoggerManager::GetInstance()->getLogger("xx");
    ZHOU_ERROR(l) << "Singleton LoggerManager";

    return 0;
}