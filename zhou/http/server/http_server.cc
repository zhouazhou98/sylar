#include "http_server.h"
#include "http_session.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"

namespace zhou {
namespace http {

static Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

HttpServer::HttpServer(     bool keepalive,
                    zhou::IOManager * worker, 
                    zhou::IOManager * accept_worker
)       : TcpServer(worker, accept_worker), m_keepalive(keepalive)
{
    m_servletDispatch.reset(new ServletDispatch);
}

void HttpServer::handleClient(Socket::ptr client_sock_fd) {
    HttpSession::ptr session(new HttpSession(client_sock_fd));

    do {
        auto req = session->recvRequest();
        if (!req) {
            ZHOU_WARN(g_logger) << "recv http request fail, errno="
                << errno << " errstr=" << strerror(errno)
                << " cliet:" << *client_sock_fd;
            break;
        }

        HttpResponse::ptr res(
            new HttpResponse(   req->getHttpVersion(),
                                req->isClose() || !m_keepalive
                            )
        );

        res->setHeader("Server", getName());
        m_servletDispatch->handle(req, res, session);
        // res->setBody(req->toString());

        ZHOU_INFO(g_logger) << *req;
        ZHOU_INFO(g_logger) << *res;

        session->sendResponse(res);
    } while (m_keepalive);

    session->close();
}


}
}
