#include "scheduler.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"


static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");

namespace zhou {

Scheduler::Scheduler(int threads, bool use_caller, const std::string & name){
}
Scheduler::~Scheduler(){
}



// 获得当前调度器
Scheduler::ptr Scheduler::GetThis(){
}
// 调度器的主协程
//      注意区分与线程的主协程的区别
Fiber::ptr Scheduler::GetMainFiber(){
}

void Scheduler::start(){
}
void Scheduler::stop(){
}





}

