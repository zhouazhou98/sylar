#ifndef __ZHOU_HTTP_RESPONSE_PARSER_H__
#define __ZHOU_HTTP_RESPONSE_PARSER_H__

#include "ragel/http11_common.h"
#include "ragel/httpclient_parser.h"
#include <memory>
#include "http_response.h"

namespace zhou {
namespace http {

class HttpResponseParser {
public:
    typedef std::shared_ptr<HttpResponseParser> ptr;
public:
    HttpResponseParser();

    size_t execute(char * data, size_t len, bool chunk);

    int isFinished();

    int hasError();

public:
    HttpResponse::ptr getData() const { return m_data; }

    uint64_t getContentLength();
    const httpclient_parser& getParser() { return m_parser; }

    void setError(int val) { m_error = val; }

public:
    static uint64_t GetHttpResponseBufferSize();

    static uint64_t GetHttpResponseMaxBodySize();

private:
    httpclient_parser m_parser;
    HttpResponse::ptr m_data;
    int m_error;

};


}
}


#endif // ! __ZHOU_HTTP_RESPONSE_PARSER_H__
