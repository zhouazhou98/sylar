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
    XX(write)        \
    XX(writev)       \
    XX(send)        \
    XX(sendto)    \
    XX(sendmsg)     
#undef XX

// 4. -------------- write function --------------
// 4.1 write
ssize_t write(int fd, const void *buf, size_t count) {
    return zhou::do_io(fd, write_hook, "write", zhou::IOManager::WRITE, SO_SNDTIMEO, buf, count);
}

// 4.2 writev
ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return zhou::do_io(fd, writev_hook, "writev", zhou::IOManager::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

// 4.3 send sendto sendmsg
ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    return zhou::do_io(sockfd, send_hook, "send", zhou::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                    const struct sockaddr *dest_addr, socklen_t addrlen) {
    return zhou::do_io(sockfd, sendto_hook, "sendto", zhou::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags, dest_addr, addrlen);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
    return zhou::do_io(sockfd, sendmsg_hook, "sendmsg", zhou::IOManager::WRITE, SO_SNDTIMEO, msg, flags);
}



#ifdef __cplusplus
}
#endif
