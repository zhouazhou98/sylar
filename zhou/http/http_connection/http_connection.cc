#include "http_connection.h"
#include "zhou/utils/macro.h"
#include "zhou/log/log_manager.h"
#include "zhou/http/http_parser/http_request/http_request_parser.h"
#include "zhou/http/http_parser/http_response/http_response_parser.h"

namespace zhou {
namespace http {

static Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

HttpConnection::HttpConnection(Socket::ptr sock, bool owner) 
        : SocketStream(sock, owner)
{
}

int HttpConnection::sendRequest(HttpRequest::ptr req) {
    std::stringstream ss;
    ss << *req;
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}

HttpResponse::ptr HttpConnection::recvResponse() {
    HttpResponseParser::ptr parser(new HttpResponseParser);
    uint64_t buff_size = HttpResponseParser::GetHttpResponseBufferSize();

    std::shared_ptr<char> buffer(
        new char[buff_size + 1], 
        [](char * ptr) {
            delete [] ptr;
        }
    );

    char * data = buffer.get();
    int offset = 0;

    do {
        int len = read(data + offset, buff_size - offset);
        if (len <= 0) {
            close();
            return nullptr;
        }

        len += offset;
        data[len] = '\0';

        size_t nparser = parser->execute(data, len, false);
        if (parser->hasError()) {
            close();
            return nullptr;
        }

        offset = len - nparser;
        if(offset == (int)buff_size) {
            close();
            return nullptr;
        }
        if (parser->isFinished()) {
            break;
        }
    } while (true);

    int64_t length = parser->getContentLength();
    if (length > 0) {
        std::string body;
        body.reserve(length);

        int len = 0;
        if (length >= offset) {
            memcpy(&body[0], data, offset);
            len = offset;
        } else {
            memcpy(&body[0], data, length);
            len = length;
        }
        length -= offset;
        if (length > 0) {
            if (readFixSize(&body[len], length) <= 0) {
                close();
                return nullptr;
            }
        }
        parser->getData()->setBody(body);
    }
    return parser->getData();


}



}
}
