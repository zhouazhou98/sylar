#include "fd_ctx.h"
#include <sys/socket.h>
#include <sys/stat.h>
#include "hook.h"



namespace zhou{

FDCtx::FDCtx(int fd) : 
        m_isInit(false),
        m_isSocket(false),
        m_sysNonblock(true),
        m_userNonblock(true),
        m_isClosed(false),
        m_fd(fd),
        m_recvTimeout(-1),
        m_sendTimeout(-1)
{
    init();
}

FDCtx::~FDCtx(){
}



void FDCtx::setTimeout(int type, uint64_t val){
    if (type == SO_RCVTIMEO) {
        m_recvTimeout = val;
    } else {
        m_sendTimeout = val;
    }

}
uint64_t FDCtx::getTimeout(int type){
    if (type == SO_RCVTIMEO) {
        return m_recvTimeout;
    } else {
        return m_sendTimeout;
    }
}



bool FDCtx::init(){
    struct stat fd_stat;
    if (fstat(m_fd, &fd_stat) == -1) {
    } else {
        m_isInit = true;
        m_isSocket = S_ISSOCK(fd_stat.st_mode);
    }

    if (m_isSocket) {
        int flags = fcntl_hook(m_fd, F_GETFL, 0);
        // 若阻塞则改为非阻塞
        if (!(flags & O_NONBLOCK)) {
            fcntl_hook(m_fd, F_SETFL, flags | O_NONBLOCK);
        }
        m_sysNonblock = true;
    } else {
        m_sysNonblock = false;
    }
    return m_isInit;
}
}