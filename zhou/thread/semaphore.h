#ifndef     __ZHOU_SEMAPHORE_H__
#define     __ZHOU_SEMAPHORE_H__

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>


namespace zhou {    // __ZHOU_SEMAPHORE_H__

class Semaphore {
public:
    Semaphore(uint32_t count = 0);

    ~Semaphore();

    void notify();  // 唤醒： 数量 - 1
    void wait();    // 等待： 数量 + 1， 数量够就执行，不够就阻塞


private:
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore & operator=(const Semaphore&) = delete;
private:
    sem_t m_semaphore;

};


}   // ! __ZHOU_SEMAPHORE_H__

#endif      // ! __ZHOU_SEMAPHORE_H__
