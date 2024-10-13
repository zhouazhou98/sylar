#ifndef __ZHOU_SOCKET_STREAM_H__
#define __ZHOU_SOCKET_STREAM_H__

#include "stream.h"
#include "zhou/net/socket.h"
#include <memory>

namespace zhou {

class SocketStream : public Stream {
public:
    typedef std::shared_ptr<SocketStream> ptr;

    SocketStream(Socket::ptr sock, bool owner = true);
    ~SocketStream();

public:
    virtual int read(void * buffer, size_t length) override;

    virtual int write(const void * buffer, size_t length) override;

    virtual void close() override;



public:
    Socket::ptr getSocket() const { return m_sock; }
    bool isConnected() const;

private:
    Socket::ptr m_sock;
    bool m_owner;


};

}

#endif // !__ZHOU_SOCKET_STREAM_H__
