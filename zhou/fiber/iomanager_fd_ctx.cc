#include "iomanager.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"

static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");

namespace zhou {
// 获取事件上下文
IOManager::FdCtx::EventCtx & IOManager::FdCtx::getContext(Event event) {
    return *(EventCtx*)0;
}
// 重置事件上下文
void IOManager::FdCtx::resetContext(EventCtx & ctx) {

}
// 触发事件
void IOManager::FdCtx::triggerEvent(Event event) {

}
}
