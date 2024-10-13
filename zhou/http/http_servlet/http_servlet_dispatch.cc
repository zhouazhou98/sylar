#include "http_servlet_dispatch.h"
#include "http_not_found_servlet.h"
#include <fnmatch.h>

namespace zhou {
namespace http {




ServletDispatch::ServletDispatch()
        : Servlet("ServletDispatch")
{
    m_default.reset(new NotFoundServlet);
}

int32_t ServletDispatch::handle(
                            zhou::http::HttpRequest::ptr request,
                            zhou::http::HttpResponse::ptr response,
                            zhou::http::HttpSession::ptr session
) {
    auto servlet = getMatchedServlet(request->getPath());
    if (servlet) {
        return servlet->handle(request, response, session);
    }
    return m_default->handle(request, response, session);
}


Servlet::ptr ServletDispatch::getServlet(const std::string & uri) {
    MutexType::ReadLock lock(m_mutex);
    auto it = m_datas.find(uri);
    return it == m_datas.end() ? nullptr : it->second;
}

Servlet::ptr ServletDispatch::getGlobServlet(const std::string & uri) {
    MutexType::ReadLock lock(m_mutex);
    for(auto it = m_globs.begin();
            it != m_globs.end(); ++it) {
        if(!fnmatch(it->first.c_str(), uri.c_str(), 0)) {
            return it->second;
        }
    }
    return nullptr;
}


Servlet::ptr ServletDispatch::getMatchedServlet(const std::string & uri) {
    MutexType::ReadLock lock(m_mutex);
    auto mit = m_datas.find(uri);
    if(mit != m_datas.end()) {
        return mit->second;
    }
    for(auto it = m_globs.begin();
            it != m_globs.end(); ++it) {
        if(!fnmatch(it->first.c_str(), uri.c_str(), 0)) {
            return it->second;
        }
    }
    return m_default;
}



// 增
void ServletDispatch::addServlet(const std::string & uri, Servlet::ptr servlet) {
    MutexType::WriteLock lock(m_mutex);
    m_datas[uri] = servlet;
}

void ServletDispatch::addGlobServlet(const std::string & uri, Servlet::ptr servlet) {
    MutexType::WriteLock lock(m_mutex);
    for(auto it = m_globs.begin();
            it != m_globs.end(); ++it) {
        if(it->first == uri) {
            m_globs.erase(it);
            break;
        }
    }
    m_globs.push_back(std::make_pair(uri, servlet));
}

    
void ServletDispatch::addServlet(const std::string & uri, FunctionServlet::callback cb) {
    MutexType::WriteLock lock(m_mutex);
    m_datas[uri].reset(new FunctionServlet(cb));
}

void ServletDispatch::addGlobServlet(const std::string & uri, FunctionServlet::callback cb) {
    addGlobServlet(uri, Servlet::ptr(new FunctionServlet(cb)));
}


// 删
void ServletDispatch::delServlet(const std::string & uri) {
    MutexType::WriteLock lock(m_mutex);
    m_datas.erase(uri);
}

void ServletDispatch::delGlobServlet(const std::string & uri) {
    MutexType::WriteLock lock(m_mutex);
    for(auto it = m_globs.begin();
            it != m_globs.end(); ++it) {
        if(it->first == uri) {
            m_globs.erase(it);
            break;
        }
    }
}




}
}
