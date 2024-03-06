
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

#define ZHOU_LOG_FMT(logger, level, fmt, ...)    \
    if (logger->getLevel() <= level)    \
        zhou::LogEventWrap(   logger, \
                        level,  \
                        zhou::LogEvent::ptr(new zhou::LogEvent(__FILE__, __LINE__, 0, syscall(SYS_gettid), zhou::GetFiberId(), time(0)) )   \
            ).format(fmt, ## __VA_ARGS__)

#define ZHOU_TRACE_FMT(logger, fmt, ...)  ZHOU_LOG_FMT(logger, zhou::LogLevel::TRACE, fmt,## __VA_ARGS__)
#define ZHOU_DEBUG_FMT(logger, fmt, ...)  ZHOU_LOG_FMT(logger, zhou::LogLevel::DEBUG, fmt,## __VA_ARGS__)
#define ZHOU_INFO_FMT(logger, fmt, ...)   ZHOU_LOG_FMT(logger, zhou::LogLevel::INFO, fmt, ## __VA_ARGS__)
#define ZHOU_WARN_FMT(logger, fmt, ...)   ZHOU_LOG_FMT(logger, zhou::LogLevel::WARN, fmt, ## __VA_ARGS__)
#define ZHOU_ERROR_FMT(logger, fmt, ...)  ZHOU_LOG_FMT(logger, zhou::LogLevel::ERROR, fmt,## __VA_ARGS__)
#define ZHOU_FATAL_FMT(logger, fmt, ...)  ZHOU_LOG_FMT(logger, zhou::LogLevel::FATAL, fmt,## __VA_ARGS__)

#endif  // !__ZHOU_UTIL_MACRO_H__