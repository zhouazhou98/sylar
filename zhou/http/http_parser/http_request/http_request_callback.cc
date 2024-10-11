#include "http_request_callback.h"
#include "http_request_parser.h"
#include "zhou/http/http_parser/http_method.h"
#include "zhou/utils/macro.h"
#include "zhou/log/log_manager.h"

#include <stddef.h>
#include <unistd.h>
#include <string.h>

namespace zhou {
namespace http {

static Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");

// GET POST PUT etc ...
void on_request_method(void * data, const char * at, size_t length) {
    HttpRequestParser * parser = static_cast<HttpRequestParser*>(data);
    HttpMethod m = CharsToHttpMethod(at);

    if (m == HttpMethod::INVALID_METHOD) {
        ZHOU_WARN(g_logger) << "invalid http request method: "
                << std::string(at, length);
        parser->setError(1000);
        return;
    }
    // parser->getData() 返回 parser 所在的 HttpRequestParser 类指针
    parser->getData()->setMethod(m);
}

void on_request_uri(void * data, const char * at, size_t length) {    

}

void on_request_fragment(void * data, const char * at, size_t length) {    
    HttpRequestParser * parser = static_cast<HttpRequestParser*>(data);
    parser->getData()->setFragment(std::string(at, length));
}

void on_request_path(void * data, const char * at, size_t length) {
    HttpRequestParser * parser = static_cast<HttpRequestParser*>(data);
    parser->getData()->setPath(std::string(at, length));
}

void on_request_query(void * data, const char * at, size_t length) {
    HttpRequestParser * parser = static_cast<HttpRequestParser*>(data);
    parser->getData()->setQuery(std::string(at, length));
}

void on_request_version(void * data, const char * at, size_t length) {
    HttpRequestParser * parser = static_cast<HttpRequestParser*>(data);
    uint8_t v = 0;
    if (strncmp(at, "HTTP/1.1", length) == 0) {
        v = 0x11;
    } else if (strncmp(at, "HTTP/1.1", length) == 0) {
        v = 0x10;
    } else {
        ZHOU_WARN(g_logger) << "invalid http request version: "
                << std::string(at, length);
        parser->setError(1001);
        return;
    }
    parser->getData()->setHttpVersion(v);
}

void on_request_header_done(void * data, const char * at, size_t length) {

}

void on_request_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen) {
    HttpRequestParser * parser = static_cast<HttpRequestParser*>(data);
    if (flen == 0) {
        ZHOU_WARN(g_logger) << "invalid http request field length == 0";
        parser->setError(1002);
        return;
    }
    parser->getData()->setHeader(std::string(field, flen), std::string(value, vlen));

}



}   // ! http namespace
}   // ! zhou namespace



