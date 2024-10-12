#include "http_response.h"
#include <sstream>

namespace zhou {
namespace http {

HttpResponse::HttpResponse(uint8_t version, bool close) 
        : m_status(HttpStatus::OK),
        m_httpVersion(version),
        m_close(close)
{

}



std::string HttpResponse::getHeader(const std::string & key, const std::string def) const {
    auto it = m_headers.find(key);
    if (it == m_headers.end()) {
        return def;
    }
    return it->second;
}


void HttpResponse::setHeader(const std::string & key, const std::string & val) {
    m_headers[key] = val;
}


void HttpResponse::delHeader(const std::string & key) {
    m_headers.erase(key);
}


std::ostream & HttpResponse::dump(std::ostream & os) const {
    os << "HTTP/"
       << ((uint32_t)(m_httpVersion >> 4))
       << "."
       << ((uint32_t)(m_httpVersion & 0x0F))
       << " "
       << (uint32_t)m_status
       << " "
       << (m_reason.empty() ? HttpStatusToString(m_status) : m_reason)
       << "\r\n";

    for(auto& i : m_headers) {
        if(strcasecmp(i.first.c_str(), "connection") == 0) {
            continue;
        }
        os << i.first << ": " << i.second << "\r\n";
    }
    os << "connection: " << (m_close ? "close" : "keep-alive") << "\r\n";

    if(!m_body.empty()) {
        os << "content-length: " << m_body.size() << "\r\n\r\n";
    } else {
        os << "\r\n";
    }
    return os;
}

std::string HttpResponse::toString() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

}
}