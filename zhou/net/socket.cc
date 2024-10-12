#include "socket.h"
#include "zhou/log/log_manager.h"
#include "zhou/hook/hook.h"
#include "zhou/hook/fd_ctx.h"
#include "zhou/hook/fd_manager.h"
#include "sys/time.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"
#include "zhou/fiber/iomanager.h"
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

namespace zhou {

static Logger::ptr g_logger = SingleLoggerManager::GetInstance()->getLogger("root");

}

// 0. 构造函数, 析构函数, static 成员函数
namespace zhou {

// 0. 构造函数 析构函数

// Socket::Socket(int family, int type, int protocol, int sockfd) :
// Socket::Socket(Address::ptr addr, int type, int protocol, int sockfd) :
//     m_sock_fd(sockfd),
//     m_family(addr->getFamily()),
//     m_type(type),
//     m_protocol(protocol),
//     m_isConnected(false)
// {
// 
// }
Socket::Socket(Address::ptr addr, int type, int protocol, int sockfd) {
    m_family = addr->getFamily();
    m_type = type;
    m_protocol = protocol;

    if (sockfd == -1) { // socket()
        m_isConnected = false;
        m_localAddress = addr;
        newSocket();
    } else {    // accept()
        if (sockfd == 0) { 
            newSocket(); 
        } else { 
            m_sock_fd = sockfd; 
        }
        m_isConnected = true;
        m_remoteAddress = addr;         // 1. 设置远端地址
        
        socklen_t addr_len = m_remoteAddress->getAddrLen();
        if (                            // 2. 设置本地地址
                ( getsockname(m_sock_fd, m_localAddress->getAddr(), &addr_len) ) == -1 
        ) {
            ZHOU_ERROR(g_logger) << "getsockname error";
        }
    }
    
}
Socket::~Socket() {
    close();
}



// 0. static 成员函数

// 创建 TCP/UDP Socket
// 创建一个 TCP socket
Socket::ptr Socket::CreateTCP(zhou::Address::ptr address) {
    Socket::ptr sock(new Socket(address, Socket::Type::TCP, 0));
    return sock;
}
// 创建一个 UDP socket
Socket::ptr Socket::CreateUDP(zhou::Address::ptr address) {
    Socket::ptr sock(new Socket(address, Socket::Type::UDP, 0));
    return sock;
}
/*
// 网卡 TCP
// 创建一个 IPv4 TCP socket
Socket::ptr CreateTCPSocket() {
    Socket::ptr sock(new Socket(Socket::Family::IPv4, Socket::Type::TCP, 0));
    return sock;

}
// 创建一个 IPv6 TCP socket
Socket::ptr CreateTCPSocket6() {
    Socket::ptr sock(new Socket(Socket::Family::IPv6, Socket::Type::TCP, 0));
    return sock;

}

// 网卡 UDP
// 创建一个 IPv4 UDP socket
Socket::ptr CreateUDPSocket() {
    Socket::ptr sock(new Socket(Socket::Family::IPv4, Socket::Type::UDP, 0));
    return sock;
}
// 创建一个 IPv6 UDP socket
Socket::ptr CreateUDPSocket6() {
    Socket::ptr sock(new Socket(Socket::Family::IPv6, Socket::Type::UDP, 0));
    return sock;
}

// UNIX 本机
// 创建一个 Unix 的 TCP socket
Socket::ptr CreateUnixTCPSocket() {
    Socket::ptr sock(new Socket(Socket::Family::UNIX, Socket::Type::UDP, 0));
    return sock;
}
// 创建一个 Unix 的 UDP socket
Socket::ptr CreateUnixUDPSocket() {
    Socket::ptr sock(new Socket(Socket::Family::UNIX, Socket::Type::UDP, 0));
    return sock;
}

*/

}




// 1. 时间： 超时相关设置
namespace zhou {

// 获取发送超时时间
int64_t Socket::getSendTimeout() {
    FDCtx::ptr ctx = SingleFDManager::GetInstance()->get(m_sock_fd);
    if(ctx) {
        return ctx->getTimeout(SO_SNDTIMEO);
    }
    return -1;

}
// 设置发送超时时间
void Socket::setSendTimeout(int64_t v) {
    struct timeval tv{int(v / 1000), int(v % 1000 * 1000)};
    setOption(SOL_SOCKET, SO_SNDTIMEO, tv);
}

// 获取接收超时时间
int64_t Socket::getRecvTimeout() {
    FDCtx::ptr ctx = SingleFDManager::GetInstance()->get(m_sock_fd);
    if (ctx) {
        return ctx->getTimeout(SO_RCVTIMEO);
    }
    return -1;
}
// 设置接收超时时间
void Socket::setRecvTimeout(int64_t v) {
    struct timeval tv{int(v / 1000), int(v % 1000 * 1000)};
    setOption(SOL_SOCKET, SO_RCVTIMEO, tv);
}

}




// 2. 选项设置
namespace zhou {

// getsockopt
bool Socket::getOption(int level, int option, void * result, socklen_t * len) {
    int rt = getsockopt(m_sock_fd, level, option, result, len);
    if (rt) {
        ZHOU_DEBUG(g_logger) << "getOption sockfd = " << m_sock_fd
            << " level = " << level << " option = " << option
            << " errno = " << errno << " errstr = " << strerror(errno);
        return false;
    }
    return true;
}

// setsockopt
bool Socket::setOption(int level, int option, const void * result, socklen_t len) {
    int rt = setsockopt(m_sock_fd, level, option, result, len);
    if (rt) {
        ZHOU_DEBUG(g_logger) << "setOption sockfd = " << m_sock_fd
            << " level = " << level << " option = " << option
            << " errno = " << errno << " errstr = " << strerror(errno);
        return false;
    }
    return true;
}

}






// 3. 流程： 初始化后的连接建立过程
namespace zhou {

// bind
bool Socket::bind() {
    // 1. socket 是否有效，无效则创建新的 socket
    if (!isValid()) {
        newSocket();
        if (!isValid()) {
            return false;
        }
    }

    // 2. 协议簇是否与 m_family 一致
    if (m_localAddress->getFamily() != m_family) {
        ZHOU_ERROR(g_logger) << "bind sock.family("
            << m_family << ") addr.family(" << m_localAddress->getFamily()
            << ") not equal, addr=" << m_localAddress->toString();
        return false;

    }

    if (::bind(m_sock_fd, m_localAddress->getAddr(), m_localAddress->getAddrLen())) {
        ZHOU_ERROR(g_logger) << "bind error errrno=" << errno
            << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}

// connect
bool Socket::connect(const Address::ptr remote_addr, uint64_t timeout_ms) {
    if (!isValid()) {
        ZHOU_ERROR(g_logger) << "connect error socket fd = -1";
        return false;
    }

    if (remote_addr->getFamily() != m_localAddress->getFamily() || remote_addr->getFamily() != m_family) {
        ZHOU_ERROR(g_logger) << "connect socket local address family: "
            << m_family << ", remote address family: " << remote_addr->getFamily()
            << "\n\tremote address = " << remote_addr->toString() 
            << ", local address = " << m_localAddress->toString();
        return false;
    }
    m_remoteAddress = remote_addr;
    if (timeout_ms == (uint64_t)-1) {
        if (::connect(m_sock_fd, m_remoteAddress->getAddr(), m_remoteAddress->getAddrLen())) {
            ZHOU_ERROR(g_logger) << "socket fd = " << m_sock_fd << ", error connect to address: " << m_remoteAddress->toString()
                << ", errno = " << errno << ", errstr = " << strerror(errno);
            close();
            return false;
        }
    } else {
        if (::connect_with_timeout(m_sock_fd, m_remoteAddress->getAddr(), m_remoteAddress->getAddrLen(), timeout_ms)) {
            ZHOU_ERROR(g_logger) << "socket fd = " << m_sock_fd << ", error connect to address: " << m_remoteAddress->toString()
                << ", timeout = " << timeout_ms
                << " ms, errno = " << errno << ", errstr = " << strerror(errno);
                close();
                return false;
        }
    }
    m_isConnected = true;
    return true;
}

// listen
bool Socket::listen(int backlog) {
    if (!isValid()) {
        ZHOU_ERROR(g_logger) << "listen error socket fd = -1";
        return false;
    }
    if (::listen(m_sock_fd, backlog)) {
        ZHOU_ERROR(g_logger) << "listen error errno = " << errno
            << ", errstr = " << strerror(errno);
            return false;
    }
    return true;
}

// accept
Socket::ptr Socket::accept() {
    Address::ptr remote_addr = Address::Create(m_localAddress->getAddr());
    // 2. accept
    socklen_t addr_len = remote_addr->getAddrLen();
    int new_sockfd = ::accept(m_sock_fd, remote_addr->getAddr(), &addr_len /*sizeof(struct sockaddr)*/);
    if (new_sockfd == -1) {
        ZHOU_ERROR(g_logger) << "error accept socket fd = " << m_sock_fd 
                << ", errno = " << errno << ", error str = " << strerror(errno);
        return nullptr;
    }
    Socket::ptr sock(new Socket(remote_addr, m_type, m_protocol, new_sockfd));
    // 3. 将 accept 得到的新的 socket fd 放入 FD Manager 中管理
    FDCtx::ptr ctx = SingleFDManager::GetInstance()->get(new_sockfd);
    if (ctx && !ctx->isClosed()) {
        return sock;
    } else {
        return nullptr;
    }

}

// close
bool Socket::close() {
    if (!m_isConnected && m_sock_fd == -1) {
        return true;
    }
    m_isConnected = false;
    if (m_sock_fd != -1) {
        ::close(m_sock_fd);
        m_sock_fd = -1;
    }
    return true;
}

}





// 4. 收发数据
namespace zhou {

// dump 输出信息到流中
std::ostream & Socket::dump(std::ostream & os) const {
    os << "[ Socket sockfd = " << m_sock_fd
        << ", is_connected = " << m_isConnected
        << ", family = " << m_family
        << ", type = " << m_type
        << ", protocol = " << m_protocol;
    if (m_localAddress) {
        os << ", local address = " << m_localAddress->toString();
    }
    if (m_remoteAddress) {
        os << ", local address = " << m_remoteAddress->toString();
    }
    os << "]";
    return os;
}

// send
//  send 函数用于在已经建立的连接上发送数据。这意味着它用在面向连接的协议（如 TCP）上，其中两个端点的连接是预先建立和维护的。
//  在 TCP 连接中，一旦连接建立，系统就知道数据应该发送到哪里（即对端的地址信息已经在连接建立时确定并保存下来）。因此，使用 send 发送数据时不需要再次指定目的地址，操作系统会自动处理数据的路由和发送。
int Socket::send(const void *buf, size_t len, int flags) {
    if (m_isConnected) {
        return ::send(m_sock_fd, buf, len, flags);
    }
    return -1;
}
int Socket::send(const iovec *bufs, size_t len, int flags) {
    if (m_isConnected) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec *)bufs;
        msg.msg_iovlen = len;
        return ::sendmsg(m_sock_fd, &msg, flags);
    }
    return -1;

}

// sendto
// sendto 函数通常用在无连接的协议（如 UDP）中，每个发送的数据包都可能目标不同的地址。在这种情况下，数据包不是发送到一个预先建立的连接，而是独立地发送到指定的地址。
// 每次使用 sendto 发送数据时，你需要指定数据包的目的地地址，因为 UDP 没有建立持久连接的概念，操作系统需要知道每个数据包应该发送到哪里。这样，每个 sendto 调用都必须包含足够的信息来独立地路由该数据包。
int Socket::sendto(const void *buf, size_t len, 
                    const Address::ptr dest_addr, int flags) {
    if (m_isConnected) {
        return ::sendto(m_sock_fd, buf, len, flags, dest_addr->getAddr(), dest_addr->getAddrLen());
    }
    return -1;
}
int Socket::sendto(const iovec *bufs, size_t len, 
                    const Address::ptr dest_addr, int flags) {

    if (m_isConnected) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec *)bufs;
        msg.msg_iovlen = len;
        msg.msg_name = dest_addr->getAddr();
        msg.msg_namelen = dest_addr->getAddrLen();
        return ::sendmsg(m_sock_fd, &msg, flags);
    }
    return -1;
}
    
// sendmsg

// recv
int Socket::recv(void *buf, size_t len, int flags) {
    if (m_isConnected) {
        return ::recv(m_sock_fd, buf, len, flags);
    }
    return -1;

}
int Socket::recv(iovec *bufs, size_t len, int flags) {
    if (m_isConnected) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec *)bufs;
        msg.msg_iovlen = len;
        return ::recvmsg(m_sock_fd, &msg, flags);
    }
    return -1;

}

// recvfrom
int Socket::recvfrom(void *buf, size_t len, 
                        const Address::ptr src_addr, int flags) {
    if (m_isConnected) {
        socklen_t addr_len = src_addr->getAddrLen();
        return ::recvfrom(m_sock_fd, buf, len, flags, src_addr->getAddr(), &addr_len);
    }
    return -1;

}
int Socket::recvfrom(iovec *bufs, size_t len, 
                        const Address::ptr src_addr, int flags) {
    if (m_isConnected) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec *)bufs;
        msg.msg_iovlen = len;
        msg.msg_name = src_addr->getAddr();
        msg.msg_namelen = src_addr->getAddrLen();
        return ::recvmsg(m_sock_fd, &msg, flags);
    }
    return -1;

}

// recvmsg


}






// 5. 取消事件
namespace zhou {

// cancelRead
bool Socket::cancelRead() {
    return zhou::IOManager::GetThis()->cancelEvent(m_sock_fd, IOManager::Event::READ);
}

// cancelWrite
bool Socket::cancelWrite() {
    return zhou::IOManager::GetThis()->cancelEvent(m_sock_fd, IOManager::Event::WRITE);

}

// cancelAccept
bool Socket::cancelAccept() {
    return zhou::IOManager::GetThis()->cancelEvent(m_sock_fd, IOManager::Event::READ);
}

// cancelAll
bool Socket::cancelAll() {
    return zhou::IOManager::GetThis()->cancelAll(m_sock_fd);

}


}





// 6. get
namespace zhou {

// isValid
bool Socket::isValid() {
    return m_sock_fd != -1;
}

// getError
int Socket::getError() {
    int error = 0;
    socklen_t len = sizeof(error);
    if (!getOption(SOL_SOCKET, SO_ERROR, &error, &len)) {
        error = errno;
    }
    return error;
}
}




// 7. private
namespace zhou{
// 初始化 socket
// void Socket::initSocket() {
// 
// }
// 创建 socket
void Socket::newSocket() {
    m_sock_fd = socket(m_family, m_type, m_protocol);
    if (m_sock_fd != -1) {
        int val = 1;
        setOption(SOL_SOCKET, SO_REUSEADDR, val);
        if (m_type == SOCK_STREAM) {
            setOption(IPPROTO_TCP, TCP_NODELAY, val);
        }
    } else {
        ZHOU_ERROR(g_logger) << "error create socket: family = " << m_family
            << ", type = " << m_type << ", protocol = " << m_protocol 
            << ", errno = " << errno << ", error info [" << strerror(errno) << "]";
    }

}
// 初始化 socket 句柄
// bool Socket::init(int sock_fd) {
// 
// }

std::ostream & operator<<(std::ostream & os, const Socket & sock) {
    return sock.dump(os);
}

}
