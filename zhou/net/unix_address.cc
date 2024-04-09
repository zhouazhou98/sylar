#include "unix_address.h"
#include <ostream>


namespace zhou {


UnixAddress::UnixAddress() {

}
// 通过 UnixSocket 路径 构造 UnixAddress
// path: UnixSocket 路径
UnixAddress::UnixAddress(const std::string & path) {

}
UnixAddress::UnixAddress(const sockaddr_un & addr) {

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

