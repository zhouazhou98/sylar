#include <zhou/zhou.h>


zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void test_socket() {
    // 1. 获取地址
    zhou::IPAddress::ptr addr = zhou::Address::LookupAnyIPAddress("www.baidu.com");
    if (addr) {
        ZHOU_INFO(g_logger) << "get address: " << addr->toString();
    } else {
        ZHOU_ERROR(g_logger) << "get address failed.";
        return;
    }

    // 2. 设置端口
    zhou::Socket::ptr sock = zhou::Socket::CreateTCP(addr);
    addr->setPort(80);
    ZHOU_INFO(g_logger) << "addr = " << addr->toString();

    // 3. connect
    if (!sock->connect(addr)) {
        ZHOU_ERROR(g_logger) << "connect " << addr->toString() << " fail";
        // return;
    } else {
        ZHOU_INFO(g_logger) << "connect " << addr->toString() << " connected";
    }

    // 4. send
    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buff, sizeof(buff));
    if(rt <= 0) {
        ZHOU_INFO(g_logger) << "send fail rt=" << rt;
        return;
    }

    // 5. receive
    std::string buffs;
    buffs.resize(4096);
    rt = sock->recv(&buffs[0], buffs.size());
    if(rt <= 0) {
        ZHOU_INFO(g_logger) << "recv fail rt=" << rt;
        return;
    }

    // 6. 输出
    buffs.resize(rt);
    ZHOU_INFO(g_logger) << buffs;
}


int main() {
    zhou::IOManager::ptr iom(new zhou::IOManager(2, false));
    iom->start();
    iom->schedule(&test_socket);
    iom->stop();
}
