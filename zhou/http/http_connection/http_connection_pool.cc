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
            (conn->getCreateTime() + m_maxKeepAliveTime) >= now_ms ||
            (conn->getRequestCount() >= m_maxRequest)
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

        // ZHOU_DEBUG(g_logger) << "addr: " << addr ? addr->toString() : "nullptr";
        
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
        ZHOU_DEBUG(g_logger) << "sock fd: " << sock->getSockFD();
        
        ptr = std::shared_ptr<HttpConnection>(new HttpConnection(sock),
            [this](HttpConnection* conn) {
                this->ReleasePtr(std::shared_ptr<HttpConnection>(conn), shared_from_this());
            });
        ++m_total;

        // ptr.reset(new HttpConnection(sock));
        // ptr = std::make_shared<HttpConnection>(sock);

        m_conns.push_back(ptr);

        ZHOU_INFO(g_logger) << "socket fd = " << sock->getSockFD();
        ZHOU_INFO(g_logger) << "ptr conn socket fd = " << ptr->getSocket()->getSockFD();
    }

    // ptr->setConnectionPool(shared_from_this());
    return ptr;

    // return HttpConnection::ptr( ptr,
    //                             std::bind(  &HttpConnectionPool::ReleasePtr, 
    //                                         std::placeholders::_1, 
    //                                         this
    //                                 )
    //     );
    // auto self = shared_from_this();
    // auto ret = std::shared_ptr<HttpConnection>(ptr.get(), [this, self](HttpConnection* conn) {
    //     this->ReleasePtr(std::shared_ptr<HttpConnection>(conn), self);
    // });
    // return ret;
}

// HttpResult::ptr HttpConnectionPool::doRequest(HttpMethod method, const std::string & url, uint64_t timeout_ms, const std::map<std::string, std::string> & headers = {}, const std::string & body = "");
HttpResult::ptr HttpConnectionPool::doRequest(HttpRequest::ptr request, uint64_t timeout_ms) {
    auto conn = getConnection();
    if (!conn) {
        return std::make_shared<HttpResult>(
            (int)HttpResult::Error::POOL_GET_CONNECTION,
            nullptr,
            "pool host: " + m_host + ", port: " + std::to_string(m_port)
        );
    }

    auto sock = conn->getSocket();
    ZHOU_INFO(g_logger) << "sock fd: " << sock->getSockFD();
    if (!sock) {
        return std::make_shared<HttpResult>(
            (int)HttpResult::Error::POOL_INVALID_CONNECTION,
            nullptr,
            "pool host:" + m_host + " port:" + std::to_string(m_port)
        );
    }

    if (!sock) {
        ZHOU_ERROR(g_logger) << "error";
    }

    sock->setRecvTimeout(timeout_ms);


    int rt = conn->sendRequest(request);
    if(rt == 0) {
        return std::make_shared<HttpResult>(
            (int)HttpResult::Error::SEND_CLOSE_BY_PEER,
            nullptr,
            "send request closed by peer: " + sock->getRemoteAddress()->toString()
        );
    } else if(rt < 0) {
        return std::make_shared<HttpResult>(
            (int)HttpResult::Error::SEND_SOCKET_ERROR,
            nullptr,
            "send request socket error errno=" + std::to_string(errno) + " errstr=" + std::string(strerror(errno)));
    }


    auto res = conn->recvResponse();
    if(!res) {
        return std::make_shared<HttpResult>(
            (int)HttpResult::Error::TIMEOUT,
            nullptr,
            "recv response timeout: " + sock->getRemoteAddress()->toString() + " timeout_ms:" + std::to_string(timeout_ms));
    }
    return std::make_shared<HttpResult>((int)HttpResult::Error::OK, res, "ok");
}

void HttpConnectionPool::ReleasePtr(HttpConnection::ptr conn, HttpConnectionPool::ptr pool) {
    conn->setRequestCount(1 + conn->getRequestCount());
    uint64_t now_ms = zhou::GetCurrentMS();

    if (
        !conn->isConnected() ||
        (conn->getCreateTime() + pool->m_maxKeepAliveTime) >= now_ms ||
        (conn->getRequestCount() >= pool->m_maxRequest)
    ) {
        --pool->m_total;
        
        return;
    }

    ZHOU_INFO(g_logger) << "Request count for current connection: " << conn->getRequestCount();


    // 连接仍然有效
    MutexType::Lock lock(pool->m_mutex);
    pool->m_conns.push_back(conn);
}


}
}
