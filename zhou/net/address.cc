#include "address.h"
#include "ip_address.h"
#include "unix_address.h"
#include "zhou/utils/util.h"
#include "zhou/utils/macro.h"
#include "zhou/log/log.h"
#include "zhou/log/log_manager.h"
#include <string>
#include <sstream>
#include <string.h>

namespace zhou {

static Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");

Address::ptr Address::Create(const sockaddr * addr) {
    if (!addr) {
        return nullptr;
    }

    Address::ptr result;
    switch (addr->sa_family)
    {
    case AF_INET:
        result.reset(new IPv4Address(*(const sockaddr_in *)addr));
        break;
    
    case AF_INET6:
        result.reset(new IPv6Address(*(const sockaddr_in6 *)addr));
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
