#ifndef __ZHOU_HTTP_REQUEST_CALLBACK_H__
#define __ZHOU_HTTP_REQUEST_CALLBACK_H__


#include <unistd.h>

namespace zhou {
namespace http {


// GET POST PUT etc ...
extern void on_request_method(void * data, const char * at, size_t length);

extern void on_request_uri(void * data, const char * at, size_t length); 

extern void on_request_fragment(void * data, const char * at, size_t length);

extern void on_request_path(void * data, const char * at, size_t length);

extern void on_request_query(void * data, const char * at, size_t length);

extern void on_request_version(void * data, const char * at, size_t length);

extern void on_request_header_done(void * data, const char * at, size_t length);

extern void on_request_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen);



}   // ! http namespace
}   // ! zhou namespace



#endif // !__ZHOU_HTTP_REQUEST_CALLBACK_H__