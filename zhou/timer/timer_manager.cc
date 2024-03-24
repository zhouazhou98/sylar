
#include "timer_manager.h"

namespace zhou {

TimerManager::TimerManager(){
}
TimerManager::~TimerManager(){
}

// 添加定时器
//  ms: 定时器执行间隔时间
//  callback: 回调函数
//  recurring: 是否循环
Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> callback, bool recurring = false){
}

// 添加条件定时器
//  weak_condition: 条件
Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> callback, std::weak_ptr<void> weak_condition, bool recurring = false){
}

// 到最近一个将要执行的定时器执行的时间间隔
uint64_t TimerManager::getNextTimer(){
}

// 获取需要执行的定时器的回调函数列表
void TimerManager::listExpiredCallback(std::vector<std::function<void()>> & callbacks){
}

// 检查是否有定时器
bool TimerManager::hasTime(){
}


// 添加定时器
//  val: 定时器
//  lock: 锁
void TimerManager::addTimer(Timer::ptr val, RWMutexType::WriteLock& lock){
}

// 当有新的定时器插入到定时器的首部，执行该函数
void TimerManager::onTimeInsertedAtFront(){
}

// 检测服务器时间是否被调后了
bool TimerManager::detectClockRollover(uint64_t now_ms){
}



void TimerManager::addTimer(Timer::ptr val, RWMutexType::WriteLock& lock) {

}




}


