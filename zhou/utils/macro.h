
#ifndef __ZHOU_UTIL_MACRO_H__
#define  __ZHOU_UTIL_MACRO_H__

#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>

#define ZHOU_LOG(logger, level)    \
    if (logger->getLevel() <= level)    \
        zhou::LogEventWrap(   logger, \
                        level,  \
                        zhou::LogEvent::ptr(new zhou::LogEvent(__FILE__, __LINE__, 0, syscall(SYS_gettid), zhou::GetFiberId(), time(0)) )   \
            ).getSS()

#define ZHOU_TRACE(logger)  ZHOU_LOG(logger, zhou::LogLevel::TRACE)
#define ZHOU_DEBUG(logger)  ZHOU_LOG(logger, zhou::LogLevel::DEBUG)
#define ZHOU_INFO(logger)   ZHOU_LOG(logger, zhou::LogLevel::INFO)
#define ZHOU_WARN(logger)   ZHOU_LOG(logger, zhou::LogLevel::WARN)
#define ZHOU_ERROR(logger)  ZHOU_LOG(logger, zhou::LogLevel::ERROR)
#define ZHOU_FATAL(logger)  ZHOU_LOG(logger, zhou::LogLevel::FATAL)

#endif  // !__ZHOU_UTIL_MACRO_H__