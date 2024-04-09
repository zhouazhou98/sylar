#ifndef __ZHOU_UNIX_ADDRESS_H__
#define __ZHOU_UNIX_ADDRESS_H__

#include <memory>
#include <sys/un.h>     // unix socket
#include "address.h"


// sun_path: 一般自己定义为 "/tmp/..." 路径，后续会作为文件， 供进程间通信
// struct sockaddr_un {
//     sa_family_t sun_family;               /* AF_UNIX */
//     char        sun_path[108];            /* socket unix network Pathname */
// };


namespace zhou {

class UnixAddress : public Address {
public:
    typedef std::shared_ptr<UnixAddress> ptr;

    UnixAddress();
    // 通过 UnixSocket 路径 构造 UnixAddress
    // path: UnixSocket 路径
    UnixAddress(const std::string & path);
    UnixAddress(const sockaddr_un & addr);

public:
// static

public:
// Address 基类 虚函数
    // 返回 sockaddr 指针， 只读
    const sockaddr * getAddr() const override;

    // 返回 sockaddr 指针， 读写
    sockaddr * getAddr() override;

    // 返回 sockaddr 长度
    const socklen_t getAddrLen() const override;

    // 可读性输出地址
    std::ostream & insert(std::ostream & os) const override;

public:
    void setAddrLen(uint32_t addr_len);

private:
    sockaddr_un m_addr;
    socklen_t m_length;
};

}

#endif  // ! __ZHOU_UNIX_ADDRESS_H__
