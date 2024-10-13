#include "http_servlet.h"
#include "http_function_servlet.h"

namespace zhou {
namespace http {


FunctionServlet::FunctionServlet(callback cb) 
        : Servlet("FunctionServlet"), m_cb(cb) 
{
}
    
int32_t FunctionServlet::handle(
                        zhou::http::HttpRequest::ptr request,
                        zhou::http::HttpResponse::ptr response,
                        zhou::http::HttpSession::ptr session
) {
    return m_cb(request, response, session);
}


}
}
