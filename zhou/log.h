// 传入参数如果过大，使用引用，如果需要更改，则使用指针
// 指针使用过程中一定要注意，是否使用智能指针

#ifndef __ZHOU_LOG_H__
#define __ZHOU_LOG_H__

#include <stdint.h>
#include <string>
#include <memory>
#include <list>
#include <fstream>
#include <sstream>
#include <vector>

namespace zhou {

class Logger;
class LogFormatter;
class LogAppender;


// 日志事件
// LogEvent 在每个输出位置都会被创建，并且会在日志输出结束后被析构
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();

    const char * getFilename() const { return m_file; }
    int32_t getLine() const { return m_line; }

    uint32_t getElapse() const { return m_elapse; }
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const { return m_fiberId; }
    uint64_t getTime() const { return m_time; }
    const std::string & getContent() const { return m_content; }
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
        UNKNOWN = 0,
        TRACE = 1,
        DEBUG = 2,
        INFO = 3,
        WARN = 4,
        ERROR = 5,
        FATAL = 6
    };

    static const char * ToString(LogLevel::Level level);
};


// 日志格式
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;

    LogFormatter(const std::string& pattern);
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    void init();
public:
    class LogFormatterItem {
    public:
        typedef std::shared_ptr<LogFormatterItem> ptr;

        // LogFormatterItem(const std::string& format = "") {}
        virtual ~LogFormatterItem() {}
        virtual void format(std::ostream & os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

private:
    std::string m_pattern;
    std::vector<LogFormatterItem::ptr> m_items;
    bool m_error;

};


// 日志输出地
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}

    // log 函数为纯虚函数，需要由子类实现该方法
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr formatter) { m_formatter = formatter; }
    LogFormatter::ptr getFormatter() const { return m_formatter; }

protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};




// 日志输出器
class Logger : public std::enable_shared_from_this<Logger> {
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

    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level level) { m_level = level; }

    const std::string& getName() const { return m_name; }
private:
    std::string m_name;
    std::list<LogAppender::ptr> m_appenders;
    LogLevel::Level m_level;

};


class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    virtual void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    
private:


};
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;

    FileLogAppender(const std::string& filename);
    virtual void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

    bool reopen();
private:
    std::string m_filename;     // 需要输出到的文件名
    std::ofstream m_filestream; // 文件流

};


}






// item 子类实现
namespace zhou {        // items class for LogFormatter::LogFormatterItem

class LevelLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    LevelLogFormatterItem(const std::string& str) {}   
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToString(level);
    }
};
class LoggerNameLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    LoggerNameLogFormatterItem(const std::string& str) {}   
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << logger->getName();
    }
};
class DateTimeLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    DateTimeLogFormatterItem(const std::string& format = "%Y:%m:%d %H:%M:%S") 
            : m_format(format) {
    }
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getTime();
    }

private:
    std::string m_format;
};
class StringLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    StringLogFormatterItem(const std::string& str)
            : /* LogFormatterItem(str), */ m_string(str) {
        }
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};
class NewLineLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    NewLineLogFormatterItem(const std::string& str) {}   
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << std::endl;
    }
};
class TabLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    TabLogFormatterItem(const std::string& str) {}   
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << "\t";
    }
};


#define XX(name)                                                                                                            \
class name ## LogFormatterItem : public LogFormatter::LogFormatterItem {                                                    \
public:                                                                                                                     \
    name ## LogFormatterItem(const std::string& str) {}                                                                     \
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {               \
        os << event->get ## name();                                                                                         \
    }                                                                                                                       \
};

XX(Filename)
XX(Line)
XX(Elapse)
XX(ThreadId)
XX(FiberId)
// XX(Time)
XX(Content)

#undef XX



/*
class MessageLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};
class ElapseLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};
class ThreadIdLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};
class FiberIdLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};
class TimeLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getTime();
    }
};
class MessageLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};
class MessageLogFormatterItem : public LogFormatter::LogFormatterItem {
public:
    void format(std::ostream & os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};
*/

}   // !items class for LogFormatter::LogFormatterItem


#endif // !__ZHOU_LOG_H__