#ifndef __ZHOU_HOOK_H__
#define __ZHOU_HOOK_H__


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



#ifdef __cplusplus
}
#endif

#endif  // !__ZHOU_HOOK_H__

