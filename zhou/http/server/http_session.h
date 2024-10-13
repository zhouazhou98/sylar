#ifndef __ZHOU_HTTP_SESSION_H__
#define __ZHOU_HTTP_SESSION_H__

#include "zhou/http/http_parser/http_request/http_request.h"
#include "zhou/http/http_parser/http_response/http_response.h"
#include "socket_stream.h"
#include <memory>

namespace zhou {
namespace http {


class HttpSession : public SocketStream {
public:
    typedef std::shared_ptr<HttpSession> ptr;

    HttpSession(Socket::ptr sock, bool owner = true);

public:
    HttpRequest::ptr recvRequest();
    int sendResponse(HttpResponse::ptr res);

};


}   // http namespace
}   // zhou namespace

#endif // ! __ZHOU_HTTP_SESSION_H__
