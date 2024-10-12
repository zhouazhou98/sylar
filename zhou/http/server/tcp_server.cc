#include "tcp_server.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"
#include <string.h>

namespace zhou {

static Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

static uint64_t s_tcp_server_read_timeout = (uint64_t)(60 * 1000 * 2);

TcpServer::TcpServer(zhou::IOManager * worker,
                zhou::IOManager * accept_worker)
        : m_worker(worker),
        m_acceptWorker(accept_worker),
        m_recvTimeout(s_tcp_server_read_timeout),
        m_name("zhou/1.0.0"),
        m_isStop(true)
{
}

TcpServer::~TcpServer() {
    for (auto & i : m_socks) {
        i->close();
    }
    m_socks.clear();
}


bool TcpServer::bind(zhou::Address::ptr addr) {
    std::vector<Address::ptr> addrs;
    std::vector<Address::ptr> fails;
    addrs.push_back(addr);
    return bind(addrs, fails);
}

bool TcpServer::bind(const std::vector<Address::ptr> & addrs, std::vector<Address::ptr> & fails) {
    for (auto & addr : addrs) {
        Socket::ptr sock = Socket::CreateTCP(addr);
        if (!sock->bind()) {
            ZHOU_ERROR(g_logger) << "bind fail errno="
                << errno << " errstr=" << strerror(errno)
                << " addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            continue;
        }

        if (!sock->listen()) {
            ZHOU_ERROR(g_logger) << "listen fail errno="
                << errno << " errstr=" << strerror(errno)
                << " addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            continue;
        }

        m_socks.push_back(sock);
    }

    if (!fails.empty()) {
        m_socks.clear();
        return false;
    }

    for (auto & i : m_socks) {
        ZHOU_INFO(g_logger) << "server bind success: " << *i;
    }
    return true;
}


bool TcpServer::start() {
    if (!m_isStop) {
        return true;
    }
    m_isStop = false;
    for (auto & sock : m_socks) {
        m_acceptWorker->schedule(std::bind(&TcpServer::startAccept, shared_from_this(), sock));
    }
    return true;
}

void TcpServer::stop() {
    m_isStop = true;
    auto self = shared_from_this();
    m_acceptWorker->schedule(
        [this, self]() {
            for (auto & sock : m_socks) {
                sock->cancelAll();
                sock->close();
            }
            m_socks.clear();
        }
    );
}


void TcpServer::startAccept(Socket::ptr sock) {
    while (!m_isStop) {
        Socket::ptr client_sock_fd = sock->accept();
        if (client_sock_fd) {
            client_sock_fd->setRecvTimeout(m_recvTimeout);
            m_worker->schedule(std::bind(
                &TcpServer::handleClient, shared_from_this(), client_sock_fd
            ));
        }  else if (errno == EAGAIN) {
            continue;
        } else {
            ZHOU_ERROR(g_logger) << "accept errno=" << errno
                << " errstr=" << strerror(errno);
        }
    }
}

void TcpServer::handleClient(Socket::ptr client_sock_fd) {
    ZHOU_INFO(g_logger) << "handleClient: " << *client_sock_fd;

    // 定义接收缓冲区
    char buffer[4096];
    ssize_t bytes_received = client_sock_fd->recv(buffer, sizeof(buffer));
    if (bytes_received > 0) {
        std::string request(buffer, bytes_received);
        ZHOU_INFO(g_logger) << "Received request:\n" << request;

        // 构造 HTTP 响应头部
        std::string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Length: " + std::to_string(bytes_received) + "\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "\r\n";

        // 将客户端发送的数据作为响应正文
        response += request;

        // 发送响应给客户端
        ssize_t bytes_sent = client_sock_fd->send(response.c_str(), response.size());
        if (bytes_sent <= 0) {
            ZHOU_ERROR(g_logger) << "发送数据失败，errno=" << errno
                                 << ", errstr=" << strerror(errno);
        }
    } else if (bytes_received == 0) {
        ZHOU_INFO(g_logger) << "客户端已关闭连接";
    } else {
        ZHOU_ERROR(g_logger) << "接收数据失败，errno=" << errno
                             << ", errstr=" << strerror(errno);
    }

    // 处理完毕，关闭客户端连接
    client_sock_fd->close();
}



}

