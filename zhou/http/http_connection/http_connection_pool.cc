#include "http_connection_pool.h"
#include "zhou/utils/util.h"
#include "zhou/utils/macro.h"
#include "zhou/log/log_manager.h"
#include "zhou/net/address.h"
#include "zhou/net/ip_address.h"

namespace zhou {
namespace http {

static Logger::ptr g_logger = SingleLoggerManager::GetInstance()->getLogger("root");


HttpConnectionPool::HttpConnectionPool(const std::string & host, const std::string & vhost, uint32_t port, uint32_t maxSize, uint32_t maxKeepAliveTime, uint32_t maxRequest)
        : m_host(host), m_vhost(vhost), m_port(port),
        m_maxSize(maxSize), m_maxKeepAliveTime(maxKeepAliveTime),
        m_maxRequest(maxRequest)
{
}


HttpConnectionPool::~HttpConnectionPool() {
    
}

HttpConnection::ptr HttpConnectionPool::getConnection() {
    uint64_t now_ms = zhou::GetCurrentMS();
    // std::list<HttpConnection::ptr> invalid_conns;
    HttpConnection::ptr ptr = nullptr;


    MutexType::Lock lock(m_mutex);

    while (!m_conns.empty()) {
        auto conn = m_conns.front();
        m_conns.pop_front();

        if (
            !conn->isConnected() ||
            (conn->getCreateTime() + m_maxKeepAliveTime) <= now_ms // ||
            // (conn->getRequestCount() >= m_maxRequest)
        ) {
            // invalid_conns.push_back(conn);
            continue;
        }

        ptr = conn;
        break;

    }

    lock.unlock();

    // m_total -= invalid_conns.size();

    if (!ptr) {
        IPAddress::ptr addr = Address::LookupAnyIPAddress(m_host);
        if (!addr) {
            ZHOU_ERROR(g_logger) << "get addr fail: " << m_host;
            return nullptr;
        }

        addr->setPort(m_port);
        Socket::ptr sock = Socket::CreateTCP(addr);
        if (!sock) {
            ZHOU_ERROR(g_logger) << "create sock fail: " << addr->toString();
            return nullptr;
        }

        if (!sock->connect(addr)) {
            ZHOU_ERROR(g_logger) << "sock connect fail: " << addr->toString();
            return nullptr;
        }
        ptr.reset(new HttpConnection(sock));

        ++m_total;
    }

    // return HttpConnection::ptr(ptr.get(), std::bind(&HttpConnectionPool::ReleasePtr, std::placeholders::_1, this));
    auto self = shared_from_this();
    return std::shared_ptr<HttpConnection>(ptr.get(), [this, self](HttpConnection* conn) {
        this->ReleasePtr(std::shared_ptr<HttpConnection>(conn), self);
    });

}

// HttpResult::ptr HttpConnectionPool::doRequest(HttpMethod method, const std::string & url, uint64_t timeout_ms, const std::map<std::string, std::string> & headers = {}, const std::string & body = "");
HttpResult::ptr HttpConnectionPool::doRequest(HttpRequest::ptr request, uint64_t timeout_ms) {
    return nullptr;
}

void HttpConnectionPool::ReleasePtr(HttpConnection::ptr conn, HttpConnectionPool::ptr pool) {

}


}
}
