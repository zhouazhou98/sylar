#ifndef __ZHOU_FD_MANAGER_H__
#define __ZHOU_FD_MANAGER_H__

#include "fd_ctx.h"
#include "zhou/thread/rwlock.h"
#include "zhou/utils/singleton.h"
#include <vector>

namespace zhou {

class FDManager {
public:
    typedef RWMutex RWMutexType;

    FDManager();
    
public:
// static

public:
    // 获取/创建 文件句柄类 FDCtx
    FDCtx::ptr get(int fd, bool auto_create = false);
    // 删除文件句柄类 FDCtx
    void del(int fd);


private:
    RWMutexType m_mutex;
    std::vector<FDCtx::ptr> m_fdCtxs;
};

typedef Singleton<FDManager> SingleFDManager;

}

#endif // !__ZHOU_FD_MANAGER_H__