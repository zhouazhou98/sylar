#include "zhou/zhou.h"
#include <vector>

zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

int main() {

    ZHOU_INFO(g_logger) << "root";

    std::vector<zhou::Address::ptr> addrs;
    in_addr addr;
    inet_aton("127.0.0.1", &addr);
    const sockaddr_in addr_in = {
            .sin_family = AF_INET,
            .sin_port = htons(8080),
            .sin_addr = addr,
        };
    addrs.push_back(std::shared_ptr<zhou::IPv4Address>(new zhou::IPv4Address(addr_in)));
    for (auto iter = addrs.begin(); iter != addrs.end(); iter++) {
        ZHOU_INFO(g_logger) << iter->get()->toString();
        ZHOU_INFO(g_logger) << iter->get()->getAddr() << "\t" << iter->get()->getAddrLen();
    }
    // auto addr = zhou::Address::Create("127.0.0.1");
    return 0;
}
