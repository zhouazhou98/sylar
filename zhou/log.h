// 传入参数如果过大，使用引用，如果需要更改，则使用指针
// 指针使用过程中一定要注意，是否使用智能指针

#ifndef __ZHOU_LOG_H__
#define __ZHOU_LOG_H__

#include <stdint.h>
#include <string>
#include <memory>
#include <list>

namespace zhou {


// 日志事件
// LogEvent 在每个输出位置都会被创建，并且会在日志输出结束后被析构
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();
private:
    const char * m_file = nullptr;  // 文件名
    int32_t m_line = 0;             // 文件行数
    uint32_t m_elapse = 0;          // 程序启动到现在的毫秒数
    uint32_t m_threadId = 0;        // 执行该 event 的线程 ID
    uint32_t m_fiberId = 0;         // 执行该 event 的协程 ID
    uint64_t m_time = 0;                // 时间戳
    std::string m_content;          // 需要输出的日志内容
};


class LogLevel {
public:
    enum Level {
        TRACE = 1,
        DEBUG = 2,
        INFO = 3,
        WARN = 4,
        ERROR = 5,
        FATAL = 6
    };
};


// 日志输出地
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}
    void log(LogLevel::Level level, LogEvent::ptr event);
private:
    LogLevel::Level m_level;
};

// 日志格式
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    std::string format(LogEvent::ptr event);
private:

};



// 日志输出器
class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;


    // 这里使用 & 传入 std::string 引用，即在传参数时不需要传入整个字符串
    Logger(const std::string & name = "root");
    void log (LogLevel::Level level, LogEvent::ptr event);

    void trace (LogEvent::ptr event);
    void debug (LogEvent::ptr event);
    void info  (LogEvent::ptr event);
    void warn  (LogEvent::ptr event);
    void error (LogEvent::ptr event);
    void fatal (LogEvent::ptr event);

    void addAppender (LogAppender::ptr appender);
    void delAppender (LogAppender::ptr appender);

    LogLevel::Level getLevel() { return m_level; }
    void setLevel(LogLevel::Level level) { m_level = level; }
private:
    std::string m_name;
    std::list<LogAppender::ptr> m_appenders;
    LogLevel::Level m_level;

};


class StdoutLogAppender : public LogAppender {};
class FileLogAppender : public LogAppender {};


}


#endif // !__ZHOU_LOG_H__