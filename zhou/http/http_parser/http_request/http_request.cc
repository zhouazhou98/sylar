#include "http_request.h"
#include "http_request_parser.h"
#include <sstream>

namespace zhou {
namespace http {

bool CaseInsensitiveLess::operator() (const std::string & lhs, const std::string & rhs) const {
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}




HttpRequest::HttpRequest(uint8_t version, bool close) 
        : m_method(HttpMethod::GET), m_httpVersion(version), m_close(close), m_path("/")
{

}

// 如果存在 XXX[key] 则返回，否则返回默认 def (default)
std::string HttpRequest::getHeader(const std::string & key, const std::string & def) const {
    auto it = m_headers.find(key);
    return it == m_headers.end() ? def : it->second;
}

std::string HttpRequest::getParam(const std::string & key, const std::string & def) const {
    auto it = m_params.find(key);
    return it == m_params.end() ? def : it->second;
}

std::string HttpRequest::getCookie(const std::string & key, const std::string & def) const {
    auto it = m_cookies.find(key);
    return it == m_cookies.end() ? def : it->second;
}


void HttpRequest::setHeader(const std::string & key, const std::string & val) {
    m_headers[key] = val;
}

void HttpRequest::setParam(const std::string & key, const std::string & val) {
    m_params[key] = val;
}

void HttpRequest::setCookie(const std::string & key, const std::string & val) {
    m_cookies[key] = val;
}


// XXX[key] 
//      存在的话， val 非空则赋值， 返回 true
//      不存在则返回 false
bool HttpRequest::hasOrSetHeader(const std::string & key, std::string * val) {
    auto it = m_headers.find(key);
    if (it == m_headers.end()) {
        return false;
    }
    if (val) {
        *val = it->second;
    }
    return true;
}

bool HttpRequest::hasOrSetParam(const std::string & key, std::string * val) {
    auto it = m_params.find(key);
    if (it == m_params.end()) {
        return false;
    }
    if (val) {
        *val = it->second;
    }
    return true;
}

bool HttpRequest::hasOrSetCookie(const std::string & key, std::string * val) {
    auto it = m_cookies.find(key);
    if (it == m_cookies.end()) {
        return false;
    }
    if (val) {
        *val = it->second;
    }
    return true;
}


void HttpRequest::delHeader(const std::string & key) {
    m_headers.erase(key);
}

void HttpRequest::delParam(const std::string & key) {
    m_params.erase(key);
}

void HttpRequest::delCookie(const std::string & key) {
    m_cookies.erase(key);
}


std::ostream & HttpRequest::dump(std::ostream & os) const {
    // Method Path?Query#Fragment Version\r\n
    // --- Headers ---
    //
    // --- body ---
    os << HttpMethodToString(m_method) << " "
       << m_path
       << (m_query.empty() ? "" : "?")
       << m_query
       << (m_fragment.empty() ? "" : "#")
       << m_fragment
       << " HTTP/"
       << ((uint32_t)(m_httpVersion >> 4))
       << "."
       << ((uint32_t)(m_httpVersion & 0x0F))
       << "\r\n";
    os << "connection: " << (m_close ? "close" : "keep-alive") << "\r\n";
    for(auto& i : m_headers) {
        if(strcasecmp(i.first.c_str(), "connection") == 0) {
            continue;
        }
        os << i.first << ":" << i.second << "\r\n";
    }

    if(!m_body.empty()) {
        os << "content-length: " << m_body.size() << "\r\n\r\n"
           << m_body;
    } else {
        os << "\r\n";
    }
    return os;
}


const std::string HttpRequest::toString() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}


}   // ! http namespace
}   // ! zhou namespace