#ifndef __ZHOU_HTTP_NOT_FOUND_SERVLET_H__
#define __ZHOU_HTTP_NOT_FOUND_SERVLET_H__

#include "http_servlet.h"
#include <memory>


namespace zhou {
namespace http {

class NotFoundServlet : public Servlet {
public:
    typedef std::shared_ptr<NotFoundServlet> ptr;

    NotFoundServlet() : Servlet("NotFoundServlet") {}

    virtual ~NotFoundServlet() {}

public:
    virtual int32_t handle(
                            zhou::http::HttpRequest::ptr request,
                            zhou::http::HttpResponse::ptr response,
                            zhou::http::HttpSession::ptr session
    ) override;

};

}
}

#endif // ! __ZHOU_HTTP_NOT_FOUND_SERVLET_H__