#ifndef __ZHOU_SOCKET_H__
#define __ZHOU_SOCKET_H__

#include <memory>
#include <sys/socket.h>
#include <sys/types.h>
// #include <bits/socket_type.h>
#include "address.h"
#include <ostream>

namespace zhou {

class Socket {
public:
    typedef std::shared_ptr<Socket> ptr;
    typedef std::weak_ptr<Socket> weak_ptr;

    // family: 协议簇 IPv4, IPv6, Unix, AX, Bluetooth, etc... 实际上是决定最终将数据从什么硬件发送出去
    // type: byte stream / datagram 使用字节流还是数据报 (不过 TCP 是使用字节流， UDP 使用数据报)
    // protocol: TCP / UDP 传输层协议
    // Socket(Address::ptr addr, int type, int protocol = 0);
    Socket(Address::ptr addr, int type, int protocol = 0, int sockfd = -1);
    ~Socket();

public:
// struct
    // socket 传输层协议类型： 字节流 / 数据报
    enum Type {
        // TCP 类型
        TCP = SOCK_STREAM,
        // UDP 类型
        UDP = SOCK_DGRAM
    };

    enum Family {
        // IPv4 socket
        IPv4 = AF_INET,
        // IPv6 socket
        IPv6 = AF_INET6,
        // Unix socket
        UNIX = AF_UNIX,
    };

public:
// static
    // 创建一个 TCP socket
    static Socket::ptr CreateTCP(zhou::Address::ptr address);
    // 创建一个 UDP socket
    static Socket::ptr CreateUDP(zhou::Address::ptr address);

/*
// 网卡 TCP
    // 创建一个 IPv4 TCP socket
    static Socket::ptr CreateTCPSocket();
    // 创建一个 IPv6 TCP socket
    static Socket::ptr CreateTCPSocket6();

// 网卡 UDP
    // 创建一个 IPv4 UDP socket
    static Socket::ptr CreateUDPSocket();
    // 创建一个 IPv6 UDP socket
    static Socket::ptr CreateUDPSocket6();

// UNIX 本机
    // 创建一个 Unix 的 TCP socket
    static Socket::ptr CreateUnixTCPSocket();
    // 创建一个 Unix 的 UDP socket
    static Socket::ptr CreateUnixUDPSocket();

*/


public:
// 1. 时间： 超时相关设置

    // 获取发送超时时间
    int64_t getSendTimeout();
    // 设置发送超时时间
    void setSendTimeout(int64_t v);

    // 获取接收超时时间
    int64_t getRecvTimeout();
    // 设置接收超时时间
    void setRecvTimeout(int64_t v);

// 2. 选项设置

    // getsockopt
    bool getOption(int level, int option, void * result, socklen_t * len);
    template <typename T>
    bool getOption(int level, int option, T & result) {
        socklen_t length = sizeof(T);
        return getOption(level, option, &result, length);
    }

    // setsockopt
    bool setOption(int level, int option, const void * result, socklen_t len);
    template <typename T>
    bool setOption(int level, int option, const T & result) {
        socklen_t length = sizeof(T);
        return setOption(level, option, &result, length);
    }

// 3. 流程： 初始化后的连接建立过程

    // bind
    bool bind();

    // connect
    bool connect(const Address::ptr remote_addr, uint64_t timeout_ms = -1);

    // listen
    bool listen(int backlog = SOMAXCONN);
    // accept
    Socket::ptr accept();

    // close
    bool close();

// 4. 收发数据

    // dump 输出信息到流中
    std::ostream & dump(std::ostream & os) const;

    //send
    int send(const void *buf, size_t len, int flags = 0);
    int send(const iovec *bufs, size_t len, int flags = 0);

    // sendto
    int sendto(const void *buf, size_t len, 
                    const Address::ptr dest_addr, int flags = 0);
    int sendto(const iovec *bufs, size_t len, 
                    const Address::ptr dest_addr, int flags = 0);
    
    // sendmsg

    // recv
    int recv(void *buf, size_t len, int flags = 0);
    int recv(iovec *bufs, size_t len, int flags = 0);

    // recvfrom
    int recvfrom(void *buf, size_t len, 
                        const Address::ptr src_addr, int flags = 0);
    int recvfrom(iovec *buf, size_t len, 
                        const Address::ptr src_addr, int flags = 0);

    // recvmsg

// 5. 取消事件

    // cancelRead
    bool cancelRead();

    // cancelWrite
    bool cancelWrite();

    // cancelAccept
    bool cancelAccept();

    // cancelAll
    bool cancelAll();

// 6. get
    const int getSockFD() const { return m_sock_fd; }
    // getFamily
    const int getFamily() const { return m_family; }

    // getType
    const int getType() const { return m_type; }

    // getProtocol
    const int getProtocol() const { return m_protocol; }

    // isConnected
    const bool getIsConnected() const { return m_isConnected; }

    // isValid
    bool isValid();

    // getError
    int getError();

    Address::ptr getLocalAddress() const { return m_localAddress; }
    Address::ptr getRemoteAddress() const { return m_remoteAddress; }




private:
    // // 初始化 socket
    // void initSocket();
    // 创建 socket
    void newSocket();
    // // 初始化 socket 句柄
    // bool init(int sock_fd);

private:
    // socket fd
    int m_sock_fd;
    // 协议簇： AF_INET, AF_INET6, AF_UNIX, AF_AX, AF_BLUETOOTH, etc... 决定最终使用哪个硬件发送出去
    int m_family;
    // 类型： byte stream / datagram
    int m_type;
    // 协议： 传输层协议
    int m_protocol;
    // 是否连接
    bool m_isConnected;
    // 本地地址
    Address::ptr m_localAddress;
    // 远端地址
    Address::ptr m_remoteAddress;
};


std::ostream & operator<<(std::ostream & os, const Socket & sock);


}

#endif // !__ZHOU_SOCKET_H__
