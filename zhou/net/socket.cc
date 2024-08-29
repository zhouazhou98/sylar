#include "socket.h"

namespace zhou {

// static Logger::ptr g_logger = SingleLoggerManager::GetInstance()->getLogger("system");




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


}
