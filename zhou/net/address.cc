#include "address.h"
#include "ip_address.h"
#include "unix_address.h"
#include "zhou/utils/util.h"
#include "zhou/utils/macro.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sstream>
#include <string.h>
#include <memory>

namespace zhou {

static Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");




// static
Address::ptr Address::Create(const sockaddr * addr) {
    if (!addr) {
        return nullptr;
    }

    Address::ptr result;
    switch (addr->sa_family)
    {
    case AF_INET:
        result = std::make_shared<IPv4Address>(*(const sockaddr_in *)addr);
        break;
    
    case AF_INET6:
        result = std::make_shared<IPv6Address>(*(const sockaddr_in6 *)addr);
        break;
    
    case AF_UNIX:
        result.reset(new UnixAddress(*(const sockaddr_un *)addr));
        break;
    
    default:
        ZHOU_ASSERT(0);
        break;
    }
    return result;
}


// 根据域名解析得到所有地址
    // result: 解析得到的地址列表
    // host: 应用层 域名
    // family: 网络层 协议簇 IPv4, IPv6, Unix domain, AX, Bluetooth, etc...
    // type: 数据传输层 字节流 byte stream / 数据报 datagram
    // protocol: 传输层协议 tcp / udp
// return: 是否解析成功
bool Address::Lookup(std::vector<Address::ptr> & result, const std::string & host,
                        int family, int type, int protocol) {
    // int getaddrinfo(const char *node, const char *service,
    //                    const struct addrinfo *hints,
    //                    struct addrinfo **res);
    
    // 1. 初始化 hints 结构体（为 getaddrinfo 做准备）
    addrinfo hints;
    hints.ai_flags = 0;
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    hints.ai_addrlen = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    // 2. 检查 IPv6 地址和服务
    std::string node;
    const char * service = NULL;
    if (!host.empty() && host[0] == '[') {
        const char * endipv6 = (const char *)memchr(host.c_str() + 1, ']', host.size() - 1);
        if (endipv6) {
            if (*(endipv6 + 1) == ':') {
                service = endipv6 + 2;                          // 分离出端口号 service
            }
            node = host.substr(1, endipv6 - host.c_str() - 1);  // 提取 IPv6 地址 node
        }
    }

    // 3. 检查主机名 node 和服务 service
    if (node.empty()) {
        service = (const char *)memchr(host.c_str(), ':', host.size());
        if (service) {
            if (!memchr(service + 1, ':', host.c_str() + host.size() - service - 1)) {
                node = host.substr(0, service - host.c_str());      // 提取主机名
                ++service;                                          // 分离出端口号
            }
        }
    }

    if (node.empty()) {
        node = host;
    }

    // 5. 调用 getaddrinfo
    addrinfo *results, *next;
    int error = getaddrinfo(node.c_str(), service, &hints, &results);
    if (error) {
        ZHOU_ERROR(g_logger) << "Address::Lookup getaddrinfo(" << host << ", "
                << family << ", " << type << ") err=" << error << " errstr=" 
                << strerror(errno);
        return false;
    }

    // 6. 处理解析结果
    next = results;
    while(next) {
        result.push_back(Create(next->ai_addr));
        next = next->ai_next;
    }

    // 7. 释放内存
    freeaddrinfo(results);
    return true;
}

// 根据域名解析得到任意一个地址
Address::ptr Address::LookupAny(const std::string & host, 
                        int family, int type, int protocol) 
{
    std::vector<Address::ptr> result;
    if (Lookup(result, host, family, type, protocol)) {
        return result[0];
    }
    return nullptr;
}
// 根据域名解析得到任意一个 IP 地址
IPAddress::ptr Address::LookupAnyIPAddress(const std::string & host, 
                    int family, int type, int protocol) 
{
    std::vector<Address::ptr> result;
    if (Lookup(result, host, family, type, protocol)) {
        // for (auto & i : result) {
            // std::cout << i->toString() << std::endl;
        // }
        for (auto & i : result) {
            IPAddress::ptr v = std::dynamic_pointer_cast<IPAddress>(i);
            if (v) {
                return v;
            }
        }
    }
    return nullptr;
}

// 返回本机所有网卡的 <网卡名, 地址, 子网掩码位数>
// bool Address::GetInterfaceAddresses(  std::multimap   <   std::string, 
//                                                         std::pair<Address::ptr, uint32_t> 
//                                                     > & result,
//                                     int family = AF_INET
// ) {
// 
// }

// 返回本机指定网卡的 <地址, 子网掩码位数>
// bool Address::GetInterfaceAddress(    std::vector<std::pair<Address::ptr, uint32_t>> & result,
//                                     std::string & iface,        // 哪个网卡
//                                     int family = AF_INET
// ) {
// 
// }














// 哪种 socket
// 返回协议簇
int Address::getFamily() const {
    return getAddr()->sa_family;
}

// 返回可读性字符串
std::string Address::toString() const {
    std::stringstream ss;
    insert(ss);
    return ss.str();
}

// 以下的比较操作有必要？？？？？？不是让代码更冗余？？？？
/*
// 比较谁更大
bool Address::operator<(const Address & rhs) const {
    // 先用前面一段长度的地址进行比较
    socklen_t minlen = std::min(getAddrLen(), rhs.getAddrLen());
    int result = memcmp(getAddr(), rhs.getAddr(), minlen);
    // < 则 true
    if(result < 0) {
        return true;
    } else if(result > 0) {
        return false;
    // 如果 == 则看谁更长度更短
    } else if(getAddrLen() < rhs.getAddrLen()) {
        return true;
    }
    return false;
}
bool Address::operator==(const Address & rhs) const {
    return getAddrLen() == rhs.getAddrLen()
        && memcmp(getAddr(), rhs.getAddr(), getAddrLen()) == 0;
}
bool Address::operator!=(const Address & rhs) const {
    // 调用 operator==()
    return !(*this == rhs);
}
*/


}
