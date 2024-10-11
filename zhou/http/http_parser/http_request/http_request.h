#ifndef __ZHOU_HTTP_REQUEST_H__
#define __ZHOU_HTTP_REQUEST_H__

#include "zhou/http/http_parser/http.h"
#include "zhou/http/http_parser/http_parser.h"
#include <memory>
#include <string>
#include <map>

namespace zhou {
namespace http {


struct CaseInsensitiveLess {
    bool operator() (const std::string & lhs, const std::string & rhs) const;
};


class HttpRequest {
public:
    typedef std::shared_ptr<HttpRequest> ptr;
    typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;    // 比较时忽略大小写
    
    HttpRequest(uint8_t version = 0x11, bool close = true);
public:
// get
    HttpMethod getMethod() const { return m_method; }
    uint8_t getHttpVersion() const { return m_httpVersion; }
    bool isClose() const { return m_close; }
    const std::string & getPath() const { return m_path; }
    const std::string & getQuery() const { return m_query; }
    const std::string & getFragment() const { return m_fragment; }
    const std::string & getBody() const { return m_body; }
    const MapType & getHeaders() const { return m_headers; }
    const MapType & getParams() const { return m_params; }
    const MapType & getCookies() const { return m_cookies; }

// set
    void setMethod(HttpMethod val) { m_method = val; }
    void setHttpVersion(uint8_t val) { m_httpVersion = val; }
    void setClose(bool val) { m_close = val; }
    void setPath(const std::string & val) { m_path = val; }
    void setQuery(const std::string & val) { m_query = val; }
    void setFragment(const std::string & val) { m_fragment = val; }
    void setBody(const std::string & val) { m_body = val; }
    void setHeaders(const MapType & val) { m_headers = val; }
    void setParams(const MapType & val) { m_params = val; }
    void setCookies(const MapType & val) { m_cookies = val; }

public:
    // 如果存在 XXX[key] 则返回，否则返回默认 def (default)
    std::string getHeader(const std::string & key, const std::string & def = "") const;
    std::string getParam(const std::string & key, const std::string & def = "") const;
    std::string getCookie(const std::string & key, const std::string & def = "") const;

    void setHeader(const std::string & key, const std::string & val);
    void setParam(const std::string & key, const std::string & val);
    void setCookie(const std::string & key, const std::string & val);

    // XXX[key] 
    //      存在的话， val 非空则赋值， 返回 true
    //      不存在则返回 false
    bool hasOrSetHeader(const std::string & key, std::string * val = nullptr);
    bool hasOrSetParam(const std::string & key, std::string * val = nullptr);
    bool hasOrSetCookie(const std::string & key, std::string * val = nullptr);

    void delHeader(const std::string & key);
    void delParam(const std::string & key);
    void delCookie(const std::string & key);

// 若 XXX[key] 不存在或转换失败，则返回 def (default)
#define XX(name, var)   \
    template <class T>  \
    T get ## name ## As(const std::string & key, const T def = T()) {  \
        return getAs(var, key, def);       \
    }       
    XX(Header, m_headers)
    XX(Param, m_params)
    XX(Cookie, m_cookies)
#undef XX
    // template <class T>
    // T getHeaderAs(const std::string & key, const T def = T());
    // template <class T>
    // T getParamAs(const std::string & key, const T def = T());
    // template <class T>
    // T getCookieAs(const std::string & key, const T def = T());
// 若 XXX[key] 不存在 或 转换失败，则 val = def ，返回 false
#define XX(name, var)   \
    template <class T>  \
    bool checkGet ## name ## As(const std::string & key, T & val, const T def = T()) {  \
        return checkGetAs(var, key, val, def);       \
    }       
    XX(Header, m_headers)
    XX(Param, m_params)
    XX(Cookie, m_cookies)
#undef XX
    // template <class T>
    // bool checkGetHeaderAs(const std::string & key, T & val, const T def = T());
    // template <class T>
    // bool checkGetParamAs(const std::string & key, T & val, const T def = T());
    // template <class T>
    // bool checkGetCookieAs(const std::string & key, T & val, const T def = T());


public:
    std::ostream & dump(std::ostream & os) const;
    const std::string toString() const;

private:
    HttpMethod m_method;
    uint8_t m_httpVersion;
    bool m_close;
    std::string m_path;
    std::string m_query;
    std::string m_fragment;
    std::string m_body;
    MapType m_headers;
    MapType m_params;
    MapType m_cookies;

};

}
}

#endif // ! __ZHOU_HTTP_REQUEST_H__
