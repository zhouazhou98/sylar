#include <zhou/zhou.h>
#include <vector>

void test_tcp_server() {
    auto addr = zhou::Address::LookupAny("0.0.0.0:8033");
    std::vector<zhou::Address::ptr> addrs;
    addrs.push_back(addr);

    zhou::TcpServer::ptr tcp_server(new zhou::TcpServer);
    std::vector<zhou::Address::ptr> fails;
    while(!tcp_server->bind(addrs, fails)) {
        sleep(1);
    }
    tcp_server->start();
}

int main() {
    zhou::IOManager::ptr iom(new zhou::IOManager(2, false));
    iom->start();
    iom->schedule(&test_tcp_server);
    iom->stop();
    return 0;
}
