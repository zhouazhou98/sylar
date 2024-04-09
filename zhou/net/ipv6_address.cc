#include "ip_address.h"
#include "zhou/zhou.h"
#include <string.h>


namespace zhou {

static Logger::ptr g_logger = SingleLoggerManager::GetInstance()->getLogger("system");


IPv6Address::ptr IPv6Address::Create(const char * address, uint16_t port) {
    IPv6Address::ptr ret(new IPv6Address);
    ret->m_addr.sin6_port = htons(port);
    int result = inet_pton(AF_INET6, address, &ret->m_addr.sin6_addr);
    if (result <= 0) {
        ZHOU_ERROR(g_logger) <<  "IPv6Address::Create(" << address << ", "
                << port << ") rt=" << result << " errno=" << errno
                << " errstr=" << strerror(errno);
        return nullptr;
    }
    return ret;
}


IPv6Address::IPv6Address() {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;

}
IPv6Address::IPv6Address(const sockaddr_in6 & address) {
    m_addr = address;
}
IPv6Address::IPv6Address(const uint8_t address[16], uint16_t port) {
    memset(&m_addr, 0, sizeof(m_addr));
    memcpy(&m_addr.sin6_addr.s6_addr, address, 16);
    m_addr.sin6_port = htons(port);
    m_addr.sin6_family = AF_INET6;
}



// Address 基类 虚函数

// 返回 sockaddr 指针， 只读
const sockaddr * IPv6Address::getAddr() const {
    return (sockaddr *)&m_addr;
}

// 返回 sockaddr 指针， 读写
sockaddr * IPv6Address::getAddr() {
    return (sockaddr *)&m_addr;
}

// 返回 sockaddr 长度
const socklen_t IPv6Address::getAddrLen() const {
    return sizeof(m_addr);
}

// 可读性输出地址
std::ostream & IPv6Address::insert(std::ostream & os) const {
    os << "[";
    uint16_t * addr = (uint16_t *)m_addr.sin6_addr.s6_addr;
    for (size_t i = 0; i < 7; i++) {
        os << htons(addr[i]) << ":";
    }
    os << htons(addr[7]) << "]:" << htons(m_addr.sin6_port);

    return os;
}

// IPAddress 基类 虚函数

// 返回端口号
uint16_t IPv6Address::getPort() const {
    return m_addr.sin6_port;
}
// 设置端口号
void IPv6Address::setPort(uint16_t port) {
    m_addr.sin6_port = htons(port);
}

// 获取该地址的广播地址
// prefix_len: 子网掩码位数
// return: 调用成功返回 IPAddress ， 失败返回 nullptr
IPAddress::ptr IPv6Address::broadcastAddress(uint32_t prefix_len) {
    return nullptr;
}

// 获取该地址的网段
IPAddress::ptr IPv6Address::networkAddress(uint32_t prefix_len) {
    return nullptr;

}

// 获取子网掩码地址
IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len) {
    return nullptr;

}


}