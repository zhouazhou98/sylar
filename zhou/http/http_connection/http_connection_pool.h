#ifndef __ZHOU_HTTP_CONNECTION_POOL_H__
#define __ZHOU_HTTP_CONNECTION_POOL_H__

#include "http_result.h"
#include "http_connection.h"
#include "zhou/thread/lock.h"
#include <memory>
#include <string>
#include <list>

namespace zhou {
namespace http {

class HttpConnectionPool : public std::enable_shared_from_this<HttpConnectionPool> {
    friend HttpConnection;
public:
    typedef std::shared_ptr<HttpConnectionPool> ptr;
    typedef Mutex MutexType;

    HttpConnectionPool(const std::string & host, const std::string & vhost, uint32_t port, uint32_t maxSize, uint32_t maxKeepAliveTime, uint32_t maxRequest);
    ~HttpConnectionPool();

public:
    HttpConnection::ptr getConnection();

public:
    // HttpResult::ptr doRequest(HttpMethod method, const std::string & url, uint64_t timeout_ms, const std::map<std::string, std::string> & headers = {}, const std::string & body = "");
    HttpResult::ptr doRequest(HttpRequest::ptr request, uint64_t timeout_ms);

private:
    static void ReleasePtr(HttpConnection::ptr conn, HttpConnectionPool::ptr pool);

private:
    // 对方服务器信息
    std::string m_host;                         // 远端服务器 域名
    std::string m_vhost;                        // 头部 Host 字段
    uint32_t m_port;                            // 端口

    // 连接相关
    uint32_t m_maxSize;                         // 连接池最大数量
    uint32_t m_maxKeepAliveTime;                // 连接池每个连接最长 KeepAlive 时间
    uint32_t m_maxRequest;                      // 每个 HTTP 连接可以执行的最大请求数

    // 并行
    MutexType m_mutex;                          // 并行

    // 连接池
    std::list<HttpConnection::ptr> m_conns;     // 连接池中的每个连接
    // 总连接数目
    std::atomic<int32_t> m_total = {0};         // 连接池中已经创建的连接总数

};

}
}

#endif // ! __ZHOU_HTTP_CONNECTION_POOL_H__
