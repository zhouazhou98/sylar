#include <zhou/zhou.h>

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

    zhou::http::HttpConnectionPool::ptr pool = std::make_shared<zhou::http::HttpConnectionPool> (
            "www.baidu.com",
            "",
            80,
            10,
            1000 * 30,
            20
    );


void test_http_connection_pool() {
    zhou::IOManager::GetThis()->addTimer(1000, 
            []() {

                zhou::http::HttpRequest::ptr req(new zhou::http::HttpRequest);
                req->setPath("/");
                req->setMethod(zhou::http::HttpMethod::GET);
                req->setHeader("Host", "www.baidu.com");

                ZHOU_INFO(g_logger) << "request";
                auto r = pool->doRequest(req, (uint64_t)300);
                ZHOU_INFO(g_logger) << r->toString();
                ZHOU_INFO(g_logger) << r->getHttpResponse()->getBody();
            }
    );

}


void test_http_connection() {
    zhou::Address::ptr addr = zhou::Address::LookupAnyIPAddress("www.baidu.com:80");
    if (!addr) {
        ZHOU_INFO(g_logger) << "get addr error";
        return;
    }

    zhou::Socket::ptr sock = zhou::Socket::CreateTCP(addr);
    bool rt = sock->connect(addr);
    if (!rt) {
        ZHOU_INFO(g_logger) << "connect " << addr->toString() << " failed";
        return;
    }

    zhou::http::HttpConnection::ptr conn(new zhou::http::HttpConnection(sock));
    zhou::http::HttpRequest::ptr req(new zhou::http::HttpRequest);

    req->setPath("/");
    req->setHeader("Host", "www.baidu.com");

    conn->sendRequest(req);
    zhou::http::HttpResponse::ptr res = conn->recvResponse();

    if(!res) {
        ZHOU_INFO(g_logger) << "recv response error";
        return;
    }

    ZHOU_INFO(g_logger) << *res;
    std::ofstream ofs("res.dat");
    ofs << *res;

}

int main() {
    zhou::IOManager::ptr iom(new zhou::IOManager(1, true));

    iom->start();
    iom->schedule(test_http_connection);
    iom->schedule(test_http_connection_pool);
    iom->stop();

    return 0;
}

