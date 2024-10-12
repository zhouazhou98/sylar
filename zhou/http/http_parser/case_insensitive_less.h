#ifndef __ZHOU_CASE_INSENSITIVE_LESS_H__
#define __ZHOU_CASE_INSENSITIVE_LESS_H__


#include <string>
#include <string.h>

namespace zhou {
namespace http {

struct CaseInsensitiveLess {
    bool operator() (const std::string & lhs, const std::string & rhs) const {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};


}   //  ! namespace http
}   //  ! namespace zhou

#endif // ! __ZHOU_CASE_INSENSITIVE_LESS_H__
