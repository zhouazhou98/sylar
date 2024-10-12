#ifndef __ZHOU_ADDRESS_H__
#define __ZHOU_ADDRESS_H__

#include <memory>
#include <sys/socket.h>
#include <string>
#include <vector>
#include <map>

// 为什么在 iomanager 中使用 管道， 而在这里又封装 unix socket 来进行进程间通信？
//  https://blog.csdn.net/qq_34999565/article/details/119533013 描述 pipe 和 unix socket 的区别

// 1. 创建套件字 
// int socket(int domain, int type, int protocol);
// 2. 绑定地址
// int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// 3. 监听
// int listen(int sockfd, int backlog);



// 如何抽象 socket 地址 ？
//  从各类型的 sockaddr_XXX 可以看出来，其事实上有两块内容：
//      1. family 标识 协议簇
//      2. 标识 本机进程 或 远端主机进程
//          IPv4: addr + port
//          IPv6: addr + port
//          Unix: 由于是本机进程间通信，因此本应标识两个进程号即可，不过 Unix 进程间通信可能不止一个进程需要使用该 socket addr 进行通信
//                  因此，这里使用文件路径给出一个公用服务， 想要从该 socket 通信的主机都可以连接进来
//              （这样不会导致互斥？？？？所以应该还是只能两个进程通信？？？？）
//              （我好像想错了，从一个 socket 入口可以产生多个连接，每个连接可以自己发送自己的消息）
//              （那如何区分这些不同的连接呢？？？ 如果两个进程同时链接到服务端，进行通信，IP 协议可以使用 port 区分，Unix 呢？？？）
//                      https://www.cnblogs.com/liwen01/p/17337905.html
//                      这个链接好像是 Unix socket 又封装了一个 label 来区分进程？？？？ 太麻烦了吧，如果这样还是过一层协议栈更方便吧



namespace zhou {

class IPAddress;

// 抽象基类， 没有自己的私有成员
class Address {
public:
    typedef std::shared_ptr<Address> ptr;
    virtual ~Address() {}

public:
// static
    // 创建一个地址
    static ptr Create(const struct sockaddr * addr);
    // 根据域名解析得到所有地址
        // result: 解析得到的地址列表
        // host: 应用层 域名
        // family: 网络层 协议簇 IPv4, IPv6, Unix domain, AX, Bluetooth, etc...
        // type: 数据传输层 字节流 byte stream / 数据报 datagram
        // protocol: 传输层协议 tcp / udp
    // return: 是否解析成功
    static bool Lookup(std::vector<Address::ptr> & result, const std::string & host,
                        int family = AF_INET, int type = 0, int protocol = 0);

    // 根据域名解析得到任意一个地址
    static Address::ptr LookupAny(const std::string & host, 
                        int family = AF_INET, int type = 0, int protocol = 0);
    // 根据域名解析得到任意一个 IP 地址
    static std::shared_ptr<IPAddress> LookupAnyIPAddress(const std::string & host, 
                        int family = AF_INET, int type = 0, int protocol = 0);

    // 返回本机所有网卡的 <网卡名, 地址, 子网掩码位数>
    static bool GetInterfaceAddresses(  std::multimap   <   std::string, 
                                                            std::pair<Address::ptr, uint32_t> 
                                                        > & result,
                                        int family = AF_INET
                                    );

    // 返回本机指定网卡的 <地址, 子网掩码位数>
    static bool GetInterfaceAddress(    std::vector<std::pair<Address::ptr, uint32_t>> & result,
                                        std::string & iface,        // 哪个网卡
                                        int family = AF_INET
                                    );

public:


    // 哪种 socket
    // 返回协议簇
    int getFamily() const;

    // 返回 sockaddr 指针， 只读
    virtual const sockaddr * getAddr() const = 0;

    // 返回 sockaddr 指针， 读写
    virtual sockaddr * getAddr() = 0;

    // 返回 sockaddr 长度
    virtual const socklen_t getAddrLen() const = 0;

    // 可读性输出地址
    virtual std::ostream & insert(std::ostream & os) const = 0;

    // 返回可读性字符串
    std::string toString() const;

    // 代码太冗余，用到了再说
    // bool operator<(const Address & rhs) const;
    // bool operator==(const Address & rhs) const;
    // bool operator!=(const Address & rhs) const;
    
private:

};

}

#endif  // ! __ZHOU_ADDRESS_H__
