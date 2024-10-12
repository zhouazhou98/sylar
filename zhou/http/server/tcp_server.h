#ifndef __ZHOU_TCP_SERVER_H__
#define __ZHOU_TCP_SERVER_H__

#include "zhou/net/socket.h"
#include "zhou/net/address.h"
#include "zhou/fiber/iomanager.h"
#include <memory>


namespace zhou {

class TcpServer : public std::enable_shared_from_this<TcpServer> {
public:
    typedef std::shared_ptr<TcpServer> ptr;

    TcpServer(zhou::IOManager * worker = zhou::IOManager::GetThis(), 
                    zhou::IOManager * accept_worker = zhou::IOManager::GetThis()
            );
    virtual ~TcpServer();


public:
// start stop 
    virtual bool bind(zhou::Address::ptr addr);
    virtual bool bind(const std::vector<Address::ptr> & addrs, std::vector<Address::ptr> & fails);

    virtual bool start();
    virtual void stop();

    
public:
// get set
    uint64_t getRecvTimeout() const { return m_recvTimeout; }
    void setRecvTimeout(uint64_t v) { m_recvTimeout = v; }

    std::string getName() const { return m_name; }
    void setName(const std::string & v) { m_name = v; }

    bool isStop() const { return m_isStop; }

protected:
// accept client && handle connection 
    virtual void startAccept(Socket::ptr sock);
    virtual void handleClient(Socket::ptr client_sock_fd);

private:
    std::vector<Socket::ptr> m_socks;
    IOManager * m_worker;
    IOManager * m_acceptWorker;
    uint64_t m_recvTimeout;
    std::string m_name;
    bool m_isStop;
};

}

#endif // ! __ZHOU_TCP_SERVER_H__
