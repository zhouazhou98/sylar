#ifndef __ZHOU_HTTP_RESPONSE_CALLBACK_H__
#define __ZHOU_HTTP_RESPONSE_CALLBACK_H__


namespace zhou {
namespace http {


void on_response_reason(void *data, const char *at, size_t length);

void on_response_status(void *data, const char *at, size_t length);

void on_response_chunk(void *data, const char *at, size_t length);

void on_response_version(void *data, const char *at, size_t length);

void on_response_header_done(void *data, const char *at, size_t length);

void on_response_last_chunk(void *data, const char *at, size_t length);

void on_response_http_field(void *data, const char *field, size_t flen
                           ,const char *value, size_t vlen);



}
}

#endif // !__ZHOU_HTTP_RESPONSE_CALLBACK_H__
