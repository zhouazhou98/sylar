#include "iomanager.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

namespace zhou {

// 获取事件上下文
//      根据 fd->event 获得 fd 对应事件的上下文
IOManager::FdCtx::EventCtx & IOManager::FdCtx::getContext(Event event) {
    switch (event) {
        case Event::READ:
            return read_ctx;
        case Event::WRITE:
            return write_ctx;
        default:
            ZHOU_ASSERT2(false, "getContext");
    }
}
// 重置事件上下文
void IOManager::FdCtx::resetContext(EventCtx & ctx) {
    ctx.callback = nullptr;
    ctx.fiber.reset();
    ctx.scheduler.reset();
}
// 触发事件
void IOManager::FdCtx::triggerEvent(Event event) {
    ZHOU_INFO(g_logger) << "Scheduler::GetMainFiber = " << Scheduler::GetMainFiber().use_count();
    // 检查当前监听的事件中是否存在 event 事件
    ZHOU_ASSERT(events & event);

    // 将 event 事件从 events 中删除
    events = (Event)(events & ~event);

    // 获得当前 event 事件对应的上下文： 1. 调度器; 2. 回调函数 / Fiber
    EventCtx & event_ctx = getContext(event);
    
    ZHOU_INFO(g_logger) << "IOManager::~IOManager : use count = " << GetThis().use_count();
    if (event_ctx.callback) {
        event_ctx.scheduler->schedule(&event_ctx.callback);
    } else {
        event_ctx.scheduler->schedule(&event_ctx.fiber);
    }
    ZHOU_INFO(g_logger) << "IOManager::~IOManager : use count = " << GetThis().use_count();
    resetContext(event_ctx);
    ZHOU_INFO(g_logger) << "IOManager::~IOManager : use count = " << GetThis().use_count();
    ZHOU_INFO(g_logger) << "Scheduler::GetMainFiber = " << Scheduler::GetMainFiber().use_count();
    return;
}

}
