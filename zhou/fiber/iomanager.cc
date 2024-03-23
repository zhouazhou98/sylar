
#include "iomanager.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include <memory>

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");

namespace zhou {
    // static thread_local IOManager::ptr t_io_manager;

// public

IOManager::IOManager(size_t thread_count, bool use_caller, const std::string & name) {
}

IOManager::~IOManager() {
}

// 1: success; 0: retry; -1: error
int IOManager::addEvent(int fd, Event event, std::function<void()> callback) {
    return 0;
}

bool IOManager::delEvent(int fd, Event event) {
    return true;
}

bool IOManager::cancelEvent(int fd, Event event) {
    return true;
}

bool IOManager::cancelAll(int fd) {
    return true;
}

// static 

IOManager::ptr IOManager::GetThis() {
    return std::dynamic_pointer_cast<IOManager>(Scheduler::GetThis());
    // return (IOManager::ptr)((IOManager *)&*(Scheduler::GetThis()));
}

// protected

void IOManager::tickle() {
}

bool IOManager::stopping() {
    return true;
}

void IOManager::idle() {
}

bool IOManager::stopping(uint64_t & time_out) {
    return true;
}

void IOManager::contextResize(size_t size) {
}

}
