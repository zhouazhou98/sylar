#ifndef __ZHOU_HTTP_CONNECTION_H__
#define __ZHOU_HTTP_CONNECTION_H__

// HttpSession ： 用来进行接收来自客户端的请求和发送回复
// HttpConnection 与之对应： 用来向服务器发送请求 接受服务器回复

#include "zhou/http/http_parser/http_request/http_request.h"
#include "zhou/http/http_parser/http_response/http_response.h"
#include "zhou/http/server/socket_stream.h"
#include <memory>

namespace zhou {
namespace http {

class HttpConnection : SocketStream {
public:
    typedef std::shared_ptr<HttpConnection> ptr;
    HttpConnection(Socket::ptr sock, bool owner = true);

public:
    int sendRequest(HttpRequest::ptr req);
    HttpResponse::ptr recvResponse();

};

}
}

#endif // ! __ZHOU_HTTP_CONNECTION_H__
