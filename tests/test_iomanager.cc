#include "zhou/zhou.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

void test_fiber() {
    ZHOU_INFO(g_logger) << "test fiber";
}


void test() {
    zhou::IOManager::ptr iom(new zhou::IOManager);
    ZHOU_INFO(g_logger) << "use count = " << iom.use_count();
    iom->start();
    ZHOU_INFO(g_logger) << "use count = " << iom.use_count();


    // iom->schedule(&test_fiber);
    // ZHOU_INFO(g_logger) << "use count = " << iom.use_count();
    // iom->schedule([]() {
    //     ZHOU_INFO(g_logger) << "test";
    // });
    // ZHOU_INFO(g_logger) << "use count = " << iom.use_count();


    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock_fd, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7890);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    // zhou::IOManager::GetThis()->addEvent(sock_fd, zhou::IOManager::READ, [](){
    //         ZHOU_INFO(g_logger) << "write callback";
    //     });
    // ZHOU_INFO(g_logger) << "use count = " << iom.use_count();
    zhou::IOManager::GetThis()->addEvent(sock_fd, zhou::IOManager::WRITE, [](){
            ZHOU_INFO(g_logger) << "write callback";
        });
    int rt = connect(sock_fd, (const sockaddr *)&addr, sizeof(addr));
    if (!rt) {

    }

    ZHOU_INFO(g_logger) << "use count = " << iom.use_count();
    // ZHOU_INFO(g_logger) << "IOManager root fiber use count: " << zhou::Scheduler::GetThis
    iom->stop();
    ZHOU_INFO(g_logger) << "use count = " << iom.use_count();
}

int main() {
    test();
    ZHOU_INFO(g_logger) << "main end";
    return 0;
}

