#include "util.h"

namespace zhou {

uint32_t GetFiberId() {
    return 0;
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

std::string BacktraceToStriing(int size, int skip, const std::string & prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); i++) {
        ss << prefix << bt[i] << "\n";
    }
    return ss.str();
}



}


