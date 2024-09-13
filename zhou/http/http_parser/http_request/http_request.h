#ifndef __ZHOU_HTTP_REQUEST_H__
#define __ZHOU_HTTP_REQUEST_H__

#include <memory>
#include <string>
#include "zhou/http/http_parser/http.h"

namespace zhou {
namespace http {

class HttpRequest {
public:
    typedef std::shared_ptr<HttpRequest> ptr;
    
public:

private:
    HttpMethod m_method;
    uint8_t m_httpVersion;
    bool m_close;
    std::string m_path;
    std::string m_query;
    std::string m_fragment;
    std::string m_body;

};

}
}

#endif // ! __ZHOU_HTTP_REQUEST_H__
