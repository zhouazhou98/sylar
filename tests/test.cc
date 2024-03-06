#include <iostream>
#include "zhou/log/log.h"
#include "zhou/utils/util.h"
#include "zhou/utils/macro.h"



int main() {
    zhou::Logger::ptr logger(new zhou::Logger);
    logger->addAppender(zhou::LogAppender::ptr(new zhou::StdoutLogAppender));

    // zhou::LogEvent::ptr event(new zhou::LogEvent(__FILE__, __LINE__, 0, syscall(SYS_gettid), 0, time(0)/*, "hello world!" */));
    // std::cout << event->getContent() << std::endl;
    // logger->log(zhou::LogLevel::INFO, event);

    ZHOU_LOG(logger, zhou::LogLevel::INFO) << "ok";
    ZHOU_ERROR(logger) << "ok";

    ZHOU_LOG_FMT(logger, zhou::LogLevel::INFO, "hello%s", "world");
    ZHOU_DEBUG_FMT(logger, "hello%s", "world");
    ZHOU_INFO_FMT(logger, "hello%s", "world");
    ZHOU_INFO_FMT(logger, "hello");
    return 0;
}