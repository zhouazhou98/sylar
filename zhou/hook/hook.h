#ifndef __ZHOU_HOOK_H__
#define __ZHOU_HOOK_H__

#include <memory>
#include <sys/types.h>

// 对于所有 IO 的 hook ， 目的是将原本阻塞调用的 IO 操作使用协程改写为 异步操作

namespace zhou {

// 当前线程是否开启 hook
bool is_hook_enable();
// 设置当前线程的 hool 状态
void set_hook_enable(bool flag);

extern uint64_t get_s_connect_timeout();

template<typename OriginFun, typename... Args>
ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
        uint32_t event, int timeout_so, Args&&... args);
}

struct timer_info {
    typedef std::shared_ptr<timer_info> ptr;
    typedef std::weak_ptr<timer_info> w_ptr;
    int is_cancled = 0;
};

#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

// 1. -------------- sleep --------------
// 1.1 sleep
// #include <unistd.h>
//  unsigned int sleep(unsigned int seconds);
typedef unsigned int (*sleep_fun_p) (unsigned int seconds);
extern sleep_fun_p sleep_hook;


// 1.2 usleep
// int usleep(useconds_t usec);
typedef int (*usleep_fun_p) (useconds_t usec);
extern usleep_fun_p usleep_hook;

// 1.3 nanosleep
// #include <time.h>
// int nanosleep(const struct timespec *req, struct timespec *rem);
typedef int (*nanosleep_fun_p) (const struct timespec *req, struct timespec *rem);
extern nanosleep_fun_p nanosleep_hook;


// 2. -------------- socket --------------
// 2.1 socket
// #include <sys/types.h>
// #include <sys/socket.h>
// int socket(int domain, int type, int protocol);
typedef int (*socket_fun_p) (int domain, int type, int protocol);
extern socket_fun_p socket_hook;

// 2.2 connet
// int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*connect_fun_p) (int sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern connect_fun_p connect_hook;
extern int  connect_with_timeout(int sockfd, const struct sockaddr * addr, socklen_t addrlen, uint64_t timeout_ms);

// 2.3 accept
// int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*accept_fun_p) (int sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern accept_fun_p accept_hook;

// 2.4 getsockopt setsockopt
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
// int getsockopt(int sockfd, int level, int optname,
//             void *optval, socklen_t *optlen);
typedef int (*getsockopt_fun_p) (int sockfd, int level, int optname,
            void *optval, socklen_t *optlen);
extern getsockopt_fun_p getsockopt_hook;

// int setsockopt(int sockfd, int level, int optname,
//             const void *optval, socklen_t optlen);
typedef int (*setsockopt_fun_p) (int sockfd, int level, int optname,
            const void *optval, socklen_t optlen);
extern setsockopt_fun_p setsockopt_hook;


// 3. -------------- read function --------------
// 3.1 read
#include <unistd.h>
// ssize_t read(int fd, void *buf, size_t count);
typedef ssize_t (*read_fun_p) (int fd, void *buf, size_t count);
extern read_fun_p read_hook;

// 3.2 readv
#include <sys/uio.h>
// ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
typedef ssize_t (*readv_fun_p) (int fd, const struct iovec *iov, int iovcnt);
extern readv_fun_p readv_hook;

// 3.3 recv, recvfrom, recvmsg
#include <sys/types.h>
#include <sys/socket.h>

// ssize_t recv(int sockfd, void *buf, size_t len, int flags);
typedef ssize_t (*recv_fun_p) (int sockfd, void *buf, size_t len, int flags);
extern recv_fun_p recv_hook;

// ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
//                         struct sockaddr *src_addr, socklen_t *addrlen);
typedef ssize_t (*recvfrom_fun_p) (int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen);
extern recvfrom_fun_p recvfrom_hook;

// ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
typedef ssize_t (*recvmsg_fun_p) (int sockfd, struct msghdr *msg, int flags);
extern recvmsg_fun_p recvmsg_hook;


// 4. -------------- write function --------------
// 4.1 write
#include <unistd.h>
// ssize_t write(int fd, const void *buf, size_t count);
typedef ssize_t (*write_fun_p) (int fd, const void *buf, size_t count);
extern write_fun_p write_hook;

// 4.2 writev
#include <sys/uio.h>
// ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
typedef ssize_t (*writev_fun_p) (int fd, const struct iovec *iov, int iovcnt);
extern writev_fun_p writev_hook;

// 4.3 send sendto sendmsg
#include <sys/types.h>
#include <sys/socket.h>
// ssize_t send(int sockfd, const void *buf, size_t len, int flags);
typedef ssize_t (*send_fun_p) (int sockfd, const void *buf, size_t len, int flags);
extern send_fun_p send_hook;

// ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
//                       const struct sockaddr *dest_addr, socklen_t addrlen);
typedef ssize_t (*sendto_fun_p) (int sockfd, const void *buf, size_t len, int flags,
                    const struct sockaddr *dest_addr, socklen_t addrlen);
extern sendto_fun_p sendto_hook;

// ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
typedef ssize_t (*sendmsg_fun_p) (int sockfd, const struct msghdr *msg, int flags);
extern sendmsg_fun_p sendmsg_hook;


// 5. -------------- fd control function --------------
// 5.1 close
#include <unistd.h>
// int close(int fd);
typedef int (*close_fun_p) (int fd);
extern close_fun_p close_hook;

// 5.2 fcntl
#include <fcntl.h>
// int fcntl(int fd, int cmd, ... /* arg */ );
typedef int (*fcntl_fun_p) (int fd, int cmd, ... /* arg */ );
extern fcntl_fun_p fcntl_hook;

// 5.3 ioctl
#include <sys/ioctl.h>
// int ioctl(int fd, unsigned long request, ...);
typedef int (*ioctl_fun_p) (int fd, unsigned long request, ...);
extern ioctl_fun_p ioctl_hook;


#ifdef __cplusplus
}
#endif

#endif  // !__ZHOU_HOOK_H__

