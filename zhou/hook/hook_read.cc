#include "hook.h"
// #include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
// #include "zhou/utils/macro.h"
// #include "zhou/fiber/fiber.h"
#include "zhou/fiber/iomanager.h"
// #include "zhou/fiber/scheduler.h"


// static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

#ifdef __cplusplus
extern "C" {
#endif

#define XX(name) name ## _fun_p name ## _hook = nullptr;
    XX(read)        \
    XX(readv)       \
    XX(recv)        \
    XX(recvfrom)    \
    XX(recvmsg)     
#undef XX


// 3. -------------- read function --------------
// 3.1 read
ssize_t read(int fd, void *buf, size_t count) {
    return zhou::do_io(fd, read_hook, "read", zhou::IOManager::READ, SO_RCVTIMEO, buf, count);
}

// 3.2 readv
ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return zhou::do_io(fd, readv_hook, "readv", zhou::IOManager::READ, SO_RCVTIMEO, iov, iovcnt);
}

// 3.3 recv, recvfrom, recvmsg
ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return zhou::do_io(sockfd, recv_hook, "recv", zhou::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
        struct sockaddr *src_addr, socklen_t *addrlen) {
    return zhou::do_io(sockfd, recvfrom_hook, "recvfrom", zhou::IOManager::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
    return zhou::do_io(sockfd, recvmsg_hook, "recvmsg", zhou::IOManager::READ, SO_RCVTIMEO, msg, flags);
}

#ifdef __cplusplus
}
#endif