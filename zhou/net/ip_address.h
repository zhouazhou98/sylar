#ifndef __ZHOU_IP_ADDRESS_H__
#define __ZHOU_IP_ADDRESS_H__

#include <memory>
#include <arpa/inet.h>
#include "address.h"

namespace zhou {

class IPAddress : public Address {
public:
    typedef std::shared_ptr<IPAddress> ptr;

public:
// static
    static ptr Create(const char * address, uint16_t port);

public:
// Address 基类

public:
    // 返回端口号
    virtual uint16_t getPort() const = 0;
    // 设置端口号
    virtual void setPort(uint16_t port) = 0;

    // 获取该地址的广播地址
    // prefix_len: 子网掩码位数
    // return: 调用成功返回 IPAddress ， 失败返回 nullptr
    virtual IPAddress::ptr broadcastAddress(uint32_t prefix_len) = 0;

    // 获取该地址的网段
    virtual IPAddress::ptr networkAddress(uint32_t prefix_len) = 0;

    // 获取子网掩码地址
    virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;

};









//           struct sockaddr_in {
//                sa_family_t    sin_family; /* address family: AF_INET */
//                in_port_t      sin_port;   /* port in network byte order */
//                struct in_addr sin_addr;   /* internet address */
//            };
// 
//            /* Internet address. */
//            struct in_addr {
//                uint32_t       s_addr;     /* address in network byte order */
//            };

class IPv4Address : public IPAddress {
public:
    typedef std::shared_ptr<IPv4Address> ptr;

    IPv4Address(const sockaddr_in & address);
    IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);

public:
// static
    static ptr Create(const char * address, uint16_t port);

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
    // 返回端口号
    uint16_t getPort() const override;
    // 设置端口号
    void setPort(uint16_t port) override;

// IPAddress 基类 虚函数
    // 获取该地址的广播地址
    // prefix_len: 子网掩码位数
    // return: 调用成功返回 IPAddress ， 失败返回 nullptr
    IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;

    // 获取该地址的网段
    IPAddress::ptr networkAddress(uint32_t prefix_len) override;

    // 获取子网掩码地址
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;

private:
    sockaddr_in m_addr;
};













//            struct sockaddr_in6 {
//                sa_family_t     sin6_family;   /* AF_INET6 */
//                in_port_t       sin6_port;     /* port number */
//                uint32_t        sin6_flowinfo; /* IPv6 flow information */
//                struct in6_addr sin6_addr;     /* IPv6 address */
//                uint32_t        sin6_scope_id; /* Scope ID (new in 2.4) */
//            };
// 
//            struct in6_addr {
//                unsigned char   s6_addr[16];   /* IPv6 address */
//            };


// 与 IPv4 不同： 构造函数， 私有成员 sock 地址类型 (IPv4 和 IPv6 的区别)
class IPv6Address : public IPAddress {
public:
    typedef std::shared_ptr<IPv6Address> ptr;

    IPv6Address();
    IPv6Address(const sockaddr_in6 & address);
    IPv6Address(const uint8_t address[16], uint16_t port = 0);

public:
// static
    static ptr Create(const char * address, uint16_t port);

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
// IPAddress 基类 虚函数
    // 返回端口号
    uint16_t getPort() const override;
    // 设置端口号
    void setPort(uint16_t port) override;

    // 获取该地址的广播地址
    // prefix_len: 子网掩码位数
    // return: 调用成功返回 IPAddress ， 失败返回 nullptr
    IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;

    // 获取该地址的网段
    IPAddress::ptr networkAddress(uint32_t prefix_len) override;

    // 获取子网掩码地址
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;

private:
    sockaddr_in6 m_addr;
};

}

#endif  // !__ZHOU_IP_ADDRESS_H__
