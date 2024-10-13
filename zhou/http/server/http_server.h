#ifndef __ZHOU_HTTP_SERVER_H__
#define __ZHOU_HTTP_SERVER_H__

#include "tcp_server.h"
#include <memory>

namespace zhou {
namespace http {

class HttpServer : public TcpServer {
public:
    typedef std::shared_ptr<HttpServer> ptr;

    HttpServer(     bool keepalive = false,
                    zhou::IOManager * worker = zhou::IOManager::GetThis(), 
                    zhou::IOManager * accept_worker = zhou::IOManager::GetThis()
            );

public:
    virtual void handleClient(Socket::ptr client_sock_fd) override;

private:
    bool m_keepalive;
};

}
}

#endif // ! __ZHOU_HTTP_SERVER_H__
