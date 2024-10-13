#include <zhou/zhou.h>

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

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
    zhou::IOManager::ptr iom(new zhou::IOManager(2, false));

    iom->start();
    iom->schedule(test_http_connection);
    iom->stop();

    return 0;
}

