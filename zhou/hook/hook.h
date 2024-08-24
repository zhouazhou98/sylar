#ifndef __ZHOU_HOOK_H__
#define __ZHOU_HOOK_H__

// 对于所有 IO 的 hook ， 目的是将原本阻塞调用的 IO 操作使用协程改写为 异步操作

namespace zhou {

// 当前线程是否开启 hook
bool is_hook_enable();
// 设置当前线程的 hool 状态
void set_hook_enable(bool flag);

}


#ifdef __cplusplus
extern "C" {
#endif

// 1. -------------- sleep --------------
// 1.1 sleep
#include <unistd.h>
//  unsigned int sleep(unsigned int seconds);
typedef unsigned int (*sleep_fun_p) (unsigned int seconds);
extern sleep_fun_p sleep_hook;


// 1.2 usleep
// int usleep(useconds_t usec);
typedef int (*usleep_fun_p) (useconds_t usec);
extern usleep_fun_p usleep_hook;

// 1.3 nanosleep
#include <time.h>
// int nanosleep(const struct timespec *req, struct timespec *rem);
typedef int (*nanosleep_fun_p) (const struct timespec *req, struct timespec *rem);
extern nanosleep_fun_p nanosleep_hook;


// 2. -------------- socket --------------
// 2.1 socket
#include <sys/types.h>
#include <sys/socket.h>
// int socket(int domain, int type, int protocol);
typedef int (*socket_fun_p) (int domain, int type, int protocol);
extern socket_fun_p socket_hook;

// 2.2 connet
// int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*connect_fun_p) (int sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern connect_fun_p connect_hook;

// 2.3 accept
// int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*accept_fun_p) (int sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern accept_fun_p accept_hook;


// 2. -------------- fd control function --------------
// 3.1 fcntl
#include <fcntl.h>
// int fcntl(int fd, int cmd, ... /* arg */ );
typedef int (*fcntl_fun_p) (int fd, int cmd, ... /* arg */ );
extern fcntl_fun_p fcntl_hook;



#ifdef __cplusplus
}
#endif

#endif  // !__ZHOU_HOOK_H__

