#include "ip_address.h"
#include "zhou/zhou.h"
#include <string.h>


namespace zhou {

static Logger::ptr g_logger = SingleLoggerManager::GetInstance()->getLogger("system");


// static
IPv4Address::ptr IPv4Address::Create(const char * address, uint16_t port) {
    IPv4Address::ptr ret(new IPv4Address);
    ret->m_addr.sin_port = htons(port);
    // 将 字符串格式的 address 转换为 网络字节序的 地址
    int result = inet_pton(AF_INET, address, &ret->m_addr.sin_addr);
    if (result <= 0) {
        ZHOU_ERROR(g_logger) <<  "IPv4Address::Create(" << address << ", "
                << port << ") rt=" << result << " errno=" << errno
                << " errstr=" << strerror(errno);
        return nullptr;
    }
    return ret;
}


// 构造函数
IPv4Address::IPv4Address(const sockaddr_in & address) {
    m_addr = address;
}
IPv4Address::IPv4Address(uint32_t address, uint16_t port) {
    // 将 m_addr 置为 0
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    m_addr.sin_addr.s_addr = htonl(address);
}



// Address 基类 虚函数

// 返回 sockaddr 指针， 只读
const sockaddr * IPv4Address::getAddr() const {
    return (sockaddr *)&m_addr;
}

// 返回 sockaddr 指针， 读写
sockaddr * IPv4Address::getAddr() {
    return (sockaddr *)&m_addr;
}

// 返回 sockaddr 长度
const socklen_t IPv4Address::getAddrLen() const {
    return sizeof(m_addr);
}

// 可读性输出地址
std::ostream & IPv4Address::insert(std::ostream & os) const {
    uint32_t addr = htonl(m_addr.sin_addr.s_addr);
    os << ((addr >> 24) & 0xff) << "."
       << ((addr >> 16) & 0xff) << "."
       << ((addr >> 8) & 0xff) << "."
       << (addr & 0xff);
    os << ":" << htons(m_addr.sin_port);
    return os;
}



// IPAddress 基类 虚函数


// 返回端口号
uint16_t IPv4Address::getPort() const {
    return m_addr.sin_port;
}
// 设置端口号
void IPv4Address::setPort(uint16_t port) {
    m_addr.sin_port = htons(port);
}

// 获取该地址的广播地址
// prefix_len: 子网掩码位数
// return: 调用成功返回 IPAddress ， 失败返回 nullptr
IPAddress::ptr IPv4Address::broadcastAddress(uint32_t prefix_len) {
    return nullptr;
}

// 获取该地址的网段
IPAddress::ptr IPv4Address::networkAddress(uint32_t prefix_len) {
    return nullptr;

}

// 获取子网掩码地址
IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len) {
    return nullptr;

}


}
