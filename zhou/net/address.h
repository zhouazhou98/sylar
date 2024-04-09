#ifndef __ZHOU_ADDRESS_H__
#define __ZHOU_ADDRESS_H__

#include <memory>
#include <sys/socket.h>
#include <string>

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

// 抽象基类， 没有自己的私有成员
class Address {
public:
    typedef std::shared_ptr<Address> ptr;
    virtual ~Address() {}

public:
// static
    static ptr Create(const sockaddr * addr);

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
