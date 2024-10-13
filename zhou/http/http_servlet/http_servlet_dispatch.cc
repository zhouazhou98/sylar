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


}
}
