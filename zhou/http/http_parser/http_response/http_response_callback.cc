#include "http_response_callback.h"
#include "http_response.h"
#include "http_response_parser.h"
#include "zhou/http/http_parser/http_status.h"
#include "zhou/utils/macro.h"
#include "zhou/log/log_manager.h"

namespace zhou {
namespace http {

static Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("system");

void on_response_reason(void *data, const char *at, size_t length) {
    HttpResponseParser * parser = static_cast<HttpResponseParser *>(data);
    parser->getData()->setReason(std::string(at, length));
}

void on_response_status(void *data, const char *at, size_t length) {
    HttpResponseParser * parser = static_cast<HttpResponseParser *>(data);
    HttpStatus status = (HttpStatus)(atoi(at));
    parser->getData()->setStatus(status);
}

void on_response_chunk(void *data, const char *at, size_t length) {
}

void on_response_version(void *data, const char *at, size_t length) {
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    uint8_t v = 0;
    if(strncmp(at, "HTTP/1.1", length) == 0) {
        v = 0x11;
    } else if(strncmp(at, "HTTP/1.0", length) == 0) {
        v = 0x10;
    } else {
        ZHOU_WARN(g_logger) << "invalid http response version: "
            << std::string(at, length);
        parser->setError(1001);
        return;
    }

    parser->getData()->setVersion(v);
}

void on_response_header_done(void *data, const char *at, size_t length) {
}

void on_response_last_chunk(void *data, const char *at, size_t length) {
}

void on_response_http_field(void *data, const char *field, size_t flen
                           ,const char *value, size_t vlen) {
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    if(flen == 0) {
        ZHOU_WARN(g_logger) << "invalid http response field length == 0";
        parser->setError(1002);
        return;
    }
    parser->getData()->setHeader(std::string(field, flen)
                                ,std::string(value, vlen));
}



}
}
