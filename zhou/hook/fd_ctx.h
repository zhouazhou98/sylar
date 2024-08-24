
#ifndef __ZHOU_FD_CTX_H__
#define __ZHOU_FD_CTX_H__

// 对于所有 IO 的抽象


#include <memory>

namespace zhou {

class FDCtx : public std::enable_shared_from_this<FDCtx> {
public:
    typedef std::shared_ptr<FDCtx> ptr;
    FDCtx(int fd);
    ~FDCtx();
public:
// static

public:
// 头文件实现
    bool isInit() const { return m_isInit; }
    bool isSocket() const { return m_isSocket; }
    bool isClosed() const { return m_isClosed; }

    void setUserNonblock(bool val) { m_userNonblock = val; }
    void setSysNonblock(bool val) { m_sysNonblock = val; }
    bool getUserNonblock() { return m_userNonblock; }
    bool getSysNonblock() { return m_sysNonblock; }


public:
// cc 文件实现
    void setTimeout(int type, uint64_t val);
    uint64_t getTimeout(int type);


private:
// function
    bool init();

private:
    // 是否初始化
    bool m_isInit: 1;
    // 是否为 socket
    bool m_isSocket: 1;
    // 是否 hook 非阻塞
    bool m_sysNonblock: 1;
    // 是否用户主动设置非阻塞
    bool m_userNonblock: 1;
    // 是否关闭
    bool m_isClosed: 1;
    // 文件句柄
    int m_fd;
    // 读超时时间毫秒
    uint64_t m_recvTimeout;
    // 写超时时间毫秒
    uint64_t m_sendTimeout;

};

}
#endif // !__ZHOU_FD_CTX_H__
