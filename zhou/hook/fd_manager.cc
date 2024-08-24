#include "fd_manager.h"

namespace zhou {


FDManager::FDManager() {
    m_fdCtxs.resize(64);
}

// 获取/创建 文件句柄类 FDCtx
FDCtx::ptr FDManager::get(int fd, bool auto_create) {
    if (fd == -1) {
        return nullptr;
    }
    {
        RWMutexType::ReadLock lock(m_mutex);
        if ((int)m_fdCtxs.size() <= fd) {
            if (!auto_create) {
                return nullptr;
            }
        } else {
            if (m_fdCtxs[fd] || !auto_create) {
                return m_fdCtxs[fd];
            }
        }
    }

    RWMutexType::WriteLock lock(m_mutex);
    FDCtx::ptr ctx(new FDCtx(fd));
    if (fd >= (int)m_fdCtxs.size()) {
        m_fdCtxs.resize(fd*1.5);
    }
    m_fdCtxs[fd] = ctx;
    return ctx;

}
// 删除文件句柄类 FDCtx
void FDManager::del(int fd) {
    RWMutexType::WriteLock lock(m_mutex);
    if ((int)m_fdCtxs.size() <= fd) {
        return;
    }
    m_fdCtxs[fd].reset();

}

}

