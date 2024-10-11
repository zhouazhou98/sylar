#ifndef __ZHOU_HTTP_PARSER_H__
#define __ZHOU_HTTP_PARSER_H__

#include "http_request/http_request.h"
#include "http_response/http_response.h"
#include <string>
#include <string.h>
#include <boost/lexical_cast.hpp>

namespace zhou {
namespace http {


template<class MapType, class T> 
bool checkGetAs(const MapType &m, const std::string &key, T &val, const T &def = T()) {
    auto it = m.find(key);
    if (it == m.end()) {
        return def;
    }
    try {
        val = boost::lexical_cast<T>(it->second);
        return false;
    } catch (...) {
        val = def;
    }
    return true;
}

template<class MapType, class T> 
bool getAs(const MapType &m, const std::string &key, const T &def = T()) {
    auto it = m.find(key);
    if (it == m.end()) {
        return def;
    }
    try {
        return boost::lexical_cast<T>(it->second);
    } catch (...) {

    }
    return def;
}



class HttpRequest;
std::ostream & operator<< (std::ostream & os, HttpRequest & req);

class HttpResponse;
std::ostream & operator<< (std::ostream & os, HttpResponse & res);

}
}


#endif // ! __ZHOU_HTTP_PARSER_H__
