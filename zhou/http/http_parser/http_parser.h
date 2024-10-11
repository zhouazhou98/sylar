#ifndef __ZHOU_HTTP_PARSER_H__
#define __ZHOU_HTTP_PARSER_H__

#include <string>

namespace zhou {
namespace http {
struct CaseInsensitiveLess {
    bool operator() (const std::string & lhs, const std::string & rhs) const;
};


template<class MapType, class T> 
bool checkGetAs(const MapType &m, const std::string &key, T &val, const T &def = T());

template<class MapType, class T> 
bool getAs(const MapType &m, const std::string &key, const T &def = T());

}
}


#endif // ! __ZHOU_HTTP_PARSER_H__
