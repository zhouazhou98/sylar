#include "http_not_found_servlet.h"

namespace zhou {
namespace http {


int32_t NotFoundServlet::handle(
                            zhou::http::HttpRequest::ptr request,
                            zhou::http::HttpResponse::ptr response,
                            zhou::http::HttpSession::ptr session
) {
    static const std::string & RESPONSE_BODY = "<html><head><title>404 Not Found"
        "</title></head><body><center><h1>404 Not Found</h1></center>"
        "<hr><center>zhou/1.0.0</center></body></html>";
    
    response->setStatus(zhou::http::HttpStatus::NOT_FOUND);
    response->setHeader("Server", "zhou/1.0.0");
    response->setHeader("Content-Type", "text/html");
    response->setBody(RESPONSE_BODY);

    return 0;
}

}
}

