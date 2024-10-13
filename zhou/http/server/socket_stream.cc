#include "socket_stream.h"


namespace zhou {

SocketStream::SocketStream(Socket::ptr sock, bool owner)
        : m_sock(sock), m_owner(owner)
{
}

SocketStream::~SocketStream() {
    if (m_owner && m_sock) {
        m_sock->close();
    }
}

int SocketStream::read(void * buffer, size_t length) {
    if (!isConnected()) {
        return -1;
    }
    return m_sock->recv(buffer, length);
}

int SocketStream::write(const void * buffer, size_t length)  {
    if (!isConnected()) {
        return -1;
    }
    return m_sock->send(buffer, length);
}

void SocketStream::close()  {
    if (m_sock) {
        m_sock->close();
    }
}



bool SocketStream::isConnected() const {
    return m_sock && m_sock->getIsConnected();
}


}
