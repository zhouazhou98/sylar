#include "http_response_parser.h"
#include "http_response.h"
#include "http_response_callback.h"


namespace zhou {
namespace http {

static uint64_t s_http_response_buffer_size = 4 * 1024;
static uint64_t s_http_response_max_body_size = 64 * 1024 * 1024;

HttpResponseParser::HttpResponseParser() 
    : m_error(0) 
{
    m_data.reset(new zhou::http::HttpResponse);
    httpclient_parser_init(&m_parser);
    m_parser.reason_phrase = on_response_reason;
    m_parser.status_code = on_response_status;
    m_parser.chunk_size = on_response_chunk;
    m_parser.http_version = on_response_version;
    m_parser.header_done = on_response_header_done;
    m_parser.last_chunk = on_response_last_chunk;
    m_parser.http_field = on_response_http_field;
    m_parser.data = this;
}



size_t HttpResponseParser::execute(char * data, size_t len, bool chunk) {
    size_t offset = httpclient_parser_execute(&m_parser, data, len, 0);
    memmove(data, data + offset, (len - offset));
    return offset;
}


int HttpResponseParser::isFinished() {
    return httpclient_parser_finish(&m_parser);
}


int HttpResponseParser::hasError() {
    return m_error || httpclient_parser_has_error(&m_parser);
}



uint64_t HttpResponseParser::getContentLength() {
    return m_data->getHeaderAs<uint64_t>("content-length", 0);
}



uint64_t HttpResponseParser::GetHttpResponseBufferSize() {
    return s_http_response_buffer_size;
}


uint64_t HttpResponseParser::GetHttpResponseMaxBodySize() {
    return s_http_response_max_body_size;
}





}
}