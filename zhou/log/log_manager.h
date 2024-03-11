#ifndef __ZHOU_LOG_MANAGER__H_
#define __ZHOU_LOG_MANAGER__H_

#include "log.h"
#include <map>
#include "zhou/utils/singleton.h"

namespace zhou{     // LOG_MANAGER



class LoggerManager {  
public:
    typedef SpinLock MutexType;
    LoggerManager() {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
    }
    Logger::ptr getLogger(std::string name) {
        Mutex::Lock lock(m_mutex);
        auto iter = m_loggers.find(name);
        if (iter != m_loggers.end()) 
            return iter->second;
        
        Logger::ptr logger(new Logger(name));
        logger->setRoot(m_root);
        m_loggers[name] = logger;
        return logger;
    }

    void init();

private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
    Mutex m_mutex;

};


typedef Singleton<LoggerManager> SingleLoggerManager;

}   // !LOG_MANAGER

#endif  // !__ZHOU_LOG_MANAGER__H_

