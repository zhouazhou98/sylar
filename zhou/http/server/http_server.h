#ifndef __ZHOU_HTTP_SERVER_H__
#define __ZHOU_HTTP_SERVER_H__

#include "tcp_server.h"
#include "zhou/http/http_servlet/http_servlet_dispatch.h"
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

    ServletDispatch::ptr getServletDispatch() const { return m_servletDispatch; }
    void setServletDisptch(ServletDispatch::ptr servlet_dispatch) { m_servletDispatch = servlet_dispatch; }

public:
    virtual void handleClient(Socket::ptr client_sock_fd) override;

private:
    bool m_keepalive;
    ServletDispatch::ptr m_servletDispatch;
};

}
}

#endif // ! __ZHOU_HTTP_SERVER_H__
