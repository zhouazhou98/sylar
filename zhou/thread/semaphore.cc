
#include "semaphore.h"
#include <semaphore.h>
#include <stdexcept>

namespace zhou {


Semaphore::Semaphore(uint32_t count) {
    if (sem_init(&m_semaphore, 0, count)) {
        throw std::logic_error("sem_init error");
    }
}

Semaphore::~Semaphore() {
    sem_destroy(&m_semaphore);
}

// 唤醒： 数量 - 1
void Semaphore::notify() {
    if (!sem_post(&m_semaphore)) {
        throw std::logic_error("sem_post error");
    }
}

// 等待： 数量 + 1， 数量够就执行，不够就阻塞
void Semaphore::wait() {
    if (!sem_wait(&m_semaphore)) {
        throw std::logic_error("sem_post error");
    }
}

}
