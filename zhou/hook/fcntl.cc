#include "hook.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include "zhou/utils/macro.h"
#include "zhou/fiber/fiber.h"
#include "zhou/fiber/iomanager.h"
#include "zhou/fiber/scheduler.h"


static zhou::Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

#ifdef __cplusplus
extern "C" {
#endif

#define XX(name) name ## _fun_p name ## _hook = nullptr;
    XX(fcntl)
#undef XX

// 3. -------------- fd control function --------------
// 3.1 fcntl
// #include <fcntl.h>
// int fcntl(int fd, int cmd, ... /* arg */ );
int fcntl(int fd, int cmd, ... /* arg */ ) {
    return 0;
}


#ifdef __cplusplus
}
#endif