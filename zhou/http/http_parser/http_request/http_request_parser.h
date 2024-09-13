#ifndef __ZHOU_HTTP_REQUEST_PARSER__
#define __ZHOU_HTTP_REQUEST_PARSER__

#include "http_request.h"
#include <memory>
#include "ragel/http11_parser.h"
#include "ragel/http11_common.h"

namespace zhou {
namespace http {

class HttpRequestParser {
public:
    typedef std::shared_ptr<HttpRequestParser> ptr;
public:
    HttpRequestParser();

    size_t execute(char * data, size_t len);

    int isFinished();

    int hasError();

public:
    HttpRequest::ptr getData() const { return m_data; }

    uint64_t getContentLength();
    const http_parser& getParser() { return m_parser; }

    void setError(int val) { m_error = val; }

private:
    HttpRequest::ptr m_data;
    http_parser m_parser;

    // 1000 : invalid method
    // 1001 : invalid version
    // 1002 : invalid field
    int m_error;
};

}
}

#endif // ! __ZHOU_HTTP_REQUEST_PARSER__
