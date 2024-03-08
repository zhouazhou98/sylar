#ifndef __ZHOU_THREAD_H__
#define __ZHOU_THREAD_H__

#include "zhou/zhou.h"
#include "semaphore.h"
#include "lock.h"

#include <thread>
#include <functional>
#include <memory>
#include <string>
#include <pthread.h>

// C++ 11 自带了 std::thread，不过其实现也是基于 pthread

namespace zhou {

class Thread : public std::enable_shared_from_this<Thread> {
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> callback, const std::string & name);
    ~Thread();

    const pid_t getId() const { return m_id; }
    const std::string& getName() const { return m_name; }

    void join();

    static Thread * GetThis();
    static std::string& GetName();  // 给日志使用
    static void SetName(const std::string & name);
    static void * run(void * arg);
private:
// 删除默认的拷贝函数
//  TODO: 理解一下这里的不能拷贝
//  为什么不能拷贝？ 视频弹幕说： 互斥量和互斥信号量都是不能拷贝的
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread & operator=(const Thread&) = delete;
private:
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_callback;
    std::string m_name;

    Semaphore m_semaphore;
};


}

#endif // !__ZHOU_THREAD_H__
