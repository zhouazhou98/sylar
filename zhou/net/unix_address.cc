#include "unix_address.h"
#include <ostream>
#include <string>


namespace zhou {

// 这里会检测到 sun_path 是 char * ， 找到 '\0' 来判断其中？？？？？？？？？？？？？
// 或者就是直接输出 [128] ?????????????????
static const size_t MAX_PATH_LEN = sizeof(((sockaddr_un *)0)->sun_path) - 1;

UnixAddress::UnixAddress() {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_length = offsetof(sockaddr_un, sun_path) + MAX_PATH_LEN;
}
// 通过 UnixSocket 路径 构造 UnixAddress
// path: UnixSocket 路径
UnixAddress::UnixAddress(const std::string & path) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_length = path.size() + 1;

    if (!path.empty() && path[0] == '\0') {
        --m_length;
    }

    if (m_length > sizeof(m_addr.sun_path)) {
        throw std::logic_error("path too long");
    }
    memcpy(m_addr.sun_path, path.c_str(), m_length);
    m_length += offsetof(sockaddr_un, sun_path);
}
UnixAddress::UnixAddress(const sockaddr_un & addr) {
    m_addr = addr;
    // TODO: 这里到底应该设置成多少？？需不需要将 family 占用的字节数也包括进来？
    m_length = strlen((char *)addr.sun_path);
}



// Address 基类 虚函数

// 返回 sockaddr 指针， 只读
const sockaddr * UnixAddress::getAddr() const {
    return (sockaddr *)&m_addr;
}

// 返回 sockaddr 指针， 读写
sockaddr * UnixAddress::getAddr() {
    return (sockaddr *)&m_addr;
}

// 返回 sockaddr 长度
const socklen_t UnixAddress::getAddrLen() const {
    return m_length;
}

// 可读性输出地址
std::ostream & UnixAddress::insert(std::ostream & os) const {
    if (m_length > offsetof(sockaddr_un, sun_path)
            && m_addr.sun_path[0] == '\0' ) {
        os << "\\0" << std::string(m_addr.sun_path + 1, 
                m_length - offsetof(sockaddr_un, sun_path) - 1);
        return os;
    }
    os << m_addr.sun_path;
    return os;
}




void UnixAddress::setAddrLen(uint32_t addr_len) {
    m_length = addr_len;
}


}

