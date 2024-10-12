#ifndef __ZHOU_HTTP_RESPONSE_H__
#define __ZHOU_HTTP_RESPONSE_H__

#include "zhou/http/http_parser/http.h"
#include "zhou/http/http_parser/http_parser.h"
#include "zhou/http/http_parser/case_insensitive_less.h"
#include <memory>
#include <map>

namespace zhou {
namespace http {



class HttpResponse {
public:
    typedef std::shared_ptr<HttpResponse> ptr;
    typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;

public:
    HttpResponse(uint8_t version = 0x11, bool close = true);

public:


public:

    HttpStatus getStatus() const { return m_status; }
    uint8_t getVersion() const { return m_httpVersion; }
    bool isClose() const { return m_close; }
    const std::string& getBody() const { return m_body; }
    const std::string& getReason() const { return m_reason; }

    void setStatus(HttpStatus v) { m_status = v; }
    void setVersion(uint8_t v) { m_httpVersion = v; }
    void setClose(bool v) { m_close = v; }
    void setBody(const std::string& v) { m_body = v; }
    void setReason(const std::string& v) { m_reason = v; }


    const MapType& getHeaders() const { return m_headers; }
    void setHeaders(const MapType& v) { m_headers = v; }

    std::string getHeader(const std::string & key, const std::string def = "") const;
    void setHeader(const std::string & key, const std::string & val);
    void delHeader(const std::string & key);

    template <class T>
    bool checkGetHeaderAs(const std::string & key, T & val, const T & def = T()) {
        return checkGetAs(m_headers, key, val, def);
    }
    template <class T>
    bool getHeaderAs(const std::string & key, const T & def = T()) {
        return getAs(m_headers, key, def);
    }

public:
    std::ostream & dump(std::ostream & os) const;
    std::string toString() const;


private:
    HttpStatus m_status;
    uint8_t m_httpVersion;
    bool m_close;
    std::string m_body;
    std::string m_reason;   // OK, Not Found, etc ...
    MapType m_headers;
};

}
}

#endif // !__ZHOU_HTTP_RESPONSE_H__
