#ifndef __ZHOU_UTIL_H__
#define  __ZHOU_UTIL_H__

#include <stdint.h>
#include <vector>
#include <string>

namespace zhou {

uint32_t GetFiberId();
void Backtrace(std::vector<std::string> & bt, int size, int skip);
std::string BacktraceToString(int size, int skip, const std::string & prefix = "");
}

#endif  // ! __ZHOU_UTIL_H__
