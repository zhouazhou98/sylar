#ifndef __ZHOU_HTTP_FUNCTION_SERVLET_H__
#define __ZHOU_HTTP_FUNCTION_SERVLET_H__

#include "http_servlet.h"
#include <memory>
#include <functional>

namespace zhou {
namespace http {

class FunctionServlet : public Servlet {
public:
    typedef std::shared_ptr<FunctionServlet> ptr;
    typedef std::function<
                            int32_t (
                                zhou::http::HttpRequest::ptr request,
                                zhou::http::HttpResponse::ptr response,
                                zhou::http::HttpSession::ptr session
                            )
                > callback;


    FunctionServlet(callback cb); 

    virtual ~FunctionServlet() {}

public:
    virtual int32_t handle(
                            zhou::http::HttpRequest::ptr request,
                            zhou::http::HttpResponse::ptr response,
                            zhou::http::HttpSession::ptr session
    ) override;

private:
    callback m_cb;
};
    
}
}

#endif // ! __ZHOU_HTTP_FUNCTION_SERVLET_H__