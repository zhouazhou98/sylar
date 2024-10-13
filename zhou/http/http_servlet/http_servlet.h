#ifndef __ZHOU_HTTP_SERVLET_H__
#define __ZHOU_HTTP_SERVLET_H__

#include "zhou/http/http_parser/http_request/http_request.h"
#include "zhou/http/http_parser/http_response/http_response.h"
#include "zhou/http/server/http_session.h"
#include <memory>
#include <string>

namespace zhou {
namespace http {
    
class Servlet {
public:
    typedef std::shared_ptr<Servlet> ptr;

    Servlet(const std::string & name) : m_name(name) {}
    virtual ~Servlet() {}

public:
    const std::string & getName() const { return m_name; }

public:
    virtual int32_t handle(
                            zhou::http::HttpRequest::ptr request,
                            zhou::http::HttpResponse::ptr response,
                            zhou::http::HttpSession::ptr session
    ) = 0;

protected:
    std::string m_name;
};

}
}

#endif // ! __ZHOU_HTTP_SERVLET_H__
