#ifndef __ZHOU_SOCKET_H__
#define __ZHOU_SOCKET_H__

#include <memory>
#include <sys/socket.h>
#include <sys/types.h>
// #include <bits/socket_type.h>
#include "address.h"

namespace zhou {

class Socket {
public:
    typedef std::shared_ptr<Socket> ptr;
    typedef std::weak_ptr<Socket> weak_ptr;

    // family: 协议簇 IPv4, IPv6, Unix, AX, Bluetooth, etc... 实际上是决定最终将数据从什么硬件发送出去
    // type: byte stream / datagram 使用字节流还是数据报 (不过 TCP 是使用字节流， UDP 使用数据报)
    // protocol: TCP / UDP 传输层协议
    Socket(int family, int type, int protocol = 0);
    Socket(Address::ptr addr, int type, int protocol = 0);
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


public:
// 1. 时间： 超时相关设置

// 2. 选项设置


// 3. 流程： 初始化后的连接建立过程

// 4. 收发数据


private:
    // 初始化 socket
    void initSocket();
    // 创建 socket
    void newSocket();
    // 初始化 socket 句柄
    bool init(int sock_fd);

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
