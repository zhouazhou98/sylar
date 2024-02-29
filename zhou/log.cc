#include "zhou/log.h"


namespace zhou {

Logger::Logger(const std::string & name = "root") 
        : m_name(name) {
}

void Logger::log (LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        for (auto iter = m_appenders.begin(); iter != m_appenders.end(); iter++)
            (*iter)->log(level, event);
    }
}

void Logger::trace (LogEvent::ptr event) {
    log(LogLevel::TRACE, event);
}
void Logger::debug (LogEvent::ptr event) {
    log(LogLevel::DEBUG, event);

}
void Logger::info  (LogEvent::ptr event) {
    log(LogLevel::INFO, event);

}
void Logger::warn  (LogEvent::ptr event) {
    log(LogLevel::WARN, event);

}
void Logger::error (LogEvent::ptr event) {
    log(LogLevel::ERROR, event);

}
void Logger::fatal (LogEvent::ptr event) {
    log(LogLevel::FATAL, event);

}

void Logger::addAppender (LogAppender::ptr appender) {
    m_appenders.push_back(appender);
}
void Logger::delAppender (LogAppender::ptr appender) {
    for (auto iter = m_appenders.begin(); iter != m_appenders.end(); iter++)
        if (*iter == appender) {
            m_appenders.erase(iter);
        }
}

}