#include "log.h"

#include <iostream>
#include <map>
#include <functional>
#include <memory>

namespace zhou{
const char * LogLevel::ToString(LogLevel::Level level) {


    switch(level) {
    
#define XX(name)            \
    case LogLevel::name:    \
        return #name;
    
    XX(TRACE)
    XX(DEBUG)
    XX(INFO)
    XX(WARN)
    XX(ERROR)
    XX(FATAL)

#undef XX    

    default:
        return "UNKNOWN";

    }
}
}


namespace zhou { // Logger


Logger::Logger(const std::string & name)
        : m_name(name),
        m_level(LogLevel::DEBUG) {
    // m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    m_formatter.reset(new LogFormatter("[%l] %m%n"));
}

void Logger::log (LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        for (auto iter = m_appenders.begin(); iter != m_appenders.end(); iter++)
            (*iter)->log(shared_from_this(), level, event);
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
    if (!appender->getFormatter()) {
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}
void Logger::delAppender (LogAppender::ptr appender) {
    for (auto iter = m_appenders.begin(); iter != m_appenders.end(); iter++)
        if (*iter == appender) {
            m_appenders.erase(iter);
        }
}

} // !Logger



// -------------------------------------------------------



namespace zhou { // LogAppender

FileLogAppender::FileLogAppender(const std::string& filename) 
        : m_filename(filename) {
}
void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        m_filestream << m_formatter->format(logger, level, event);
    }
}
bool FileLogAppender::reopen() {
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}

void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        std::cout << m_formatter->format(logger, level, event);
        
    }

}



} // !LogAppender



// -------------------------------------------------------



namespace zhou { // LogFormatter

LogFormatter::LogFormatter(const std::string& pattern) 
        : m_pattern(pattern) {
    init();
}
std::string LogFormatter::format(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;
    for (auto i : m_items) {
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

// %xxx     %xxx{xxxx-xx-xx}    %%
void LogFormatter::init() {
/*
    //str, format, type
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

*/
    // str, format, type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    for (size_t i = 0; i < m_pattern.size(); ++i) {
        if (m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if (i + 1 < m_pattern.size()) {
            if (m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                ++i;
                continue;
            }
        }

        int fmt_status = 0;
        size_t n = i + 1;
        std::string str;
        std::string fmt;
        size_t fmt_begin;
        while(n < m_pattern.size()) {
            if (isspace(m_pattern[n])) {
                break;
            }
            if (fmt_status == 0) {
                if (m_pattern[n] == '{') {
                    str = m_pattern.substr(i+1, n-i-1);
                    fmt_status = 1;     // 解析格式
                    ++n;
                    fmt_begin = n;
                    continue;
                }
                
            }
            if (fmt_status == 1) {
                if (m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin, n - fmt_begin - 1);
                    fmt_status = 2;
                    break;
                }
            }
        }

        // % 后只有 str 没有指定 {} 中的格式
        if (fmt_status == 0) {
            if (!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, "", 0));
            }
            str = m_pattern.substr(i+1, n-i-1);
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        } else if (fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 1));
        } else if (fmt_status == 2) {

            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        }

    }
    if (!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

    static std::map<std::string, std::function<LogFormatter::LogFormatterItem::ptr(const std::string& fmt)>> s_format_items = {
#define XX(str, C)    \
        {#str, [](const std::string& fmt) { return LogFormatter::LogFormatterItem::ptr(new C ## LogFormatterItem(fmt)); } }

        XX(m, Content),           //m: 消息
        XX(p, Level),             //p: 日志级别
        XX(r, Elapse),            //r: 累计毫秒数
        XX(c, LoggerName),        //c: 日志名称
        XX(t, ThreadId),          //t: 线程id
        XX(n, NewLine),           //n: 换行
        XX(d, DateTime),          //d: 时间
        XX(f, Filename),          //f: 文件名
        XX(l, Line),              //l: 行号
        XX(T, Tab),               //T: Tab
        XX(F, FiberId),           //F: 协程id
        //XX(N, ThreadName),        //N: 线程名称
#undef XX
    };

    for (auto& i : vec) {
        if (std::get<2>(i) == 0) {
            m_items.push_back(LogFormatterItem::ptr(
                                    new StringLogFormatterItem(std::get<0>(i))
                            ) );
        } else {
            auto iter = s_format_items.find(std::get<0>(i));
            if (iter == s_format_items.end()) {
                m_items.push_back(LogFormatterItem::ptr(
                                    new StringLogFormatterItem("<<format_error: %" + std::get<0>(i) + ">>")
                            ) );
                m_error = true;
            } else {
                m_items.push_back(iter->second(std::get<1>(i)));
            }
        }
        // std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) << std::endl;
    }
}

} // !LogFormatter



namespace zhou {

    LogEvent::LogEvent(   const char* filename, int32_t line, uint32_t elapse, 
                uint32_t threadId, uint32_t fiberId, uint64_t time,
                const std::string& content
    ) : m_filename(filename), m_line(line), m_elapse(elapse), m_threadId(threadId),
    m_fiberId(fiberId), m_time(time), m_content(content) {
    }

}


