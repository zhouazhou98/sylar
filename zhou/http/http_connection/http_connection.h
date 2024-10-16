#ifndef __ZHOU_HTTP_CONNECTION_H__
#define __ZHOU_HTTP_CONNECTION_H__

// HttpSession ： 用来进行接收来自客户端的请求和发送回复
// HttpConnection 与之对应： 用来向服务器发送请求 接受服务器回复

#include "zhou/http/http_parser/http_request/http_request.h"
#include "zhou/http/http_parser/http_response/http_response.h"
#include "zhou/http/server/socket_stream.h"
#include "zhou/thread/lock.h"
#include <memory>


namespace zhou {
namespace http {

class HttpConnectionPool;

class HttpConnection : public SocketStream, std::enable_shared_from_this<HttpConnection> {
friend HttpConnectionPool;
public:
    typedef std::shared_ptr<HttpConnection> ptr;
    typedef Mutex MutexType;
    HttpConnection(Socket::ptr sock, bool owner = true);
    ~HttpConnection();

public:
    int sendRequest(HttpRequest::ptr req);
    HttpResponse::ptr recvResponse();

public:
    // void setConnectionPool(const std::shared_ptr<HttpConnectionPool>& pool) {
    //     m_pool = pool;
    // }

public:
    uint64_t getCreateTime() const { return m_createTime; }
    void setCreateTime(uint64_t val) { m_createTime = val; }
    uint64_t getRequestCount() const { return m_requestCount; }
    void setRequestCount(uint64_t val) { m_requestCount = val; }

private:
    uint64_t m_createTime = 0;
    uint64_t m_requestCount = 0;    // 该单个连接请求的次数
    // std::weak_ptr<HttpConnectionPool> m_pool;

};

}
}

#endif // ! __ZHOU_HTTP_CONNECTION_H__
