#include "util.h"
#include "zhou/fiber/fiber.h"

namespace zhou {

uint32_t GetFiberId() {
    return zhou::Fiber::GetFiberId();
}
}



#include <string>
#include <string.h>
#include <assert.h>
#include <vector>
#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "zhou/zhou.h"

namespace zhou {


static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");


void Backtrace(std::vector<std::string> & bt, int size, int skip) {
    void ** array = (void **)malloc(sizeof(void *) * size);
    int nptrs = backtrace(array, size);

    char ** strings = backtrace_symbols(array, nptrs);
    if (strings == NULL) {
        ZHOU_ERROR(g_logger) << "Backtrace symbols error";
        free(array);
        return ;
    }

    for (int i = skip; i < nptrs; i++) {
        bt.push_back(strings[i]);
    }
    free(strings);
    free(array);
}

std::string BacktraceToString(int size, int skip, const std::string & prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); i++) {
        ss << prefix << bt[i] << "\n";
    }
    return ss.str();
}




// 获取当前毫秒数
uint64_t GetCurrentMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    // struct timeval { 
    // time_t       tv_sec;     秒      * 1000 ul   = 毫秒
    // suseconds_t  tv_usec;    微妙    / 1000 ul    = 毫秒
    // };
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}
// 获取当前微妙数
uint64_t GetCurrentUS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    // struct timeval { 
    // time_t       tv_sec;     秒      * 1000 ul   = 毫秒
    // suseconds_t  tv_usec;    微妙    / 1000 ul    = 毫秒
    // };
    return tv.tv_sec * 1000ul * 1000ul + tv.tv_usec;
}


}


