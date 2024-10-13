#ifndef __ZHOU_HTTP_SERVLET_DISPATCH_H__
#define __ZHOU_HTTP_SERVLET_DISPATCH_H__

#include "http_servlet.h"
#include "http_function_servlet.h"
#include "zhou/thread/rwlock.h"
#include <memory>
#include <vector>
#include <unordered_map>

namespace zhou {
namespace http {

class ServletDispatch : public Servlet {
public:
    typedef std::shared_ptr<ServletDispatch> ptr;
    typedef RWMutex MutexType;

    ServletDispatch();
    virtual ~ServletDispatch() {}
public:
    virtual int32_t handle(
                            zhou::http::HttpRequest::ptr request,
                            zhou::http::HttpResponse::ptr response,
                            zhou::http::HttpSession::ptr session
    ) override;

public:
// 根据 uri 查找 servlet 进行消息处理
    Servlet::ptr getServlet(const std::string & uri);
    Servlet::ptr getGlobServlet(const std::string & uri);
    Servlet::ptr getMatchedServlet(const std::string & uri);

public:
    void addServlet(const std::string & uri, Servlet::ptr servlet);
    void addGlobServlet(const std::string & uri, Servlet::ptr servlet);
    
    void addServlet(const std::string & uri, FunctionServlet::callback cb);
    void addGlobServlet(const std::string & uri, FunctionServlet::callback cb);

    void delServlet(const std::string & uri);
    void delGlobServlet(const std::string & uri);

    Servlet::ptr getDefaultServlet() const { return m_default; }
    void getDefaultServlet(Servlet::ptr servlet) { m_default = servlet; }

private:
    MutexType m_mutex;
    // 通配符
    // uri ( /zhou/abc ) -> servlet
    std::unordered_map<std::string, Servlet::ptr> m_datas;
    // uri ( /zhou/a* ) -> servlet
    std::vector<std::pair<std::string, Servlet::ptr>> m_globs;
    // 默认 servlet
    Servlet::ptr m_default;
};

}
}

#endif // ! __ZHOU_HTTP_SERVLET_DISPATCH_H__
