#include "tcp_server.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"
#include <string.h>

namespace zhou {

static Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");

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
        }  else {
            ZHOU_ERROR(g_logger) << "accept errno=" << errno
                << " errstr=" << strerror(errno);
        }
    }
}

void TcpServer::handleClient(Socket::ptr client_sock_fd) {
    ZHOU_INFO(g_logger) << "handleClient: " << *client_sock_fd;
}


}

