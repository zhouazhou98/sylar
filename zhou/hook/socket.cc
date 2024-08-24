#include "hook.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"


static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

#ifdef __cplusplus
extern "C" {
#endif

#define XX(name) name ## _fun_p name ## _hook = nullptr;
    XX(socket)
    XX(connect)
    XX(accept)
#undef XX

// extern bool is_hook_enable();

// 2. -------------- socket --------------
// 2.1 socket
// int socket(int domain, int type, int protocol);
int socket(int domain, int type, int protocol) {
    if (!zhou::is_hook_enable()) {
        return socket_hook(domain, type, protocol);
    }
    int sock_fd = socket_hook(domain, type, protocol);
    if (sock_fd == -1) {
        return sock_fd;
    }

    return 0;
}

// 2.2 connet
// int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {

    return 0;
}

// 2.3 accept
// int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {

    return 0;
}



#ifdef __cplusplus
}
#endif
