#include "http_connection.h"
#include "zhou/utils/macro.h"
#include "zhou/log/log_manager.h"
#include "zhou/http/http_parser/http_request/http_request_parser.h"
#include "zhou/http/http_parser/http_response/http_response_parser.h"
#include "http_connection_pool.h"

namespace zhou {
namespace http {

static Logger::ptr g_logger = zhou::SingleLoggerManager::GetInstance()->getLogger("root");

HttpConnection::HttpConnection(Socket::ptr sock, bool owner) 
        : SocketStream(sock, owner)
{
}
HttpConnection::~HttpConnection() {
    auto pool = m_pool.lock();
    if (pool) {
        HttpConnectionPool::ReleasePtr(shared_from_this(), pool);
    }
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

    auto client_parser = parser->getParser();
    if (client_parser.chunked) {
        std::string body;
        int len = offset;
        do {
            do {
                int rt = read(data + len, buff_size - len);
                if (rt <= 0) {
                    close();
                    return nullptr;
                }

                len += rt;
                data[len] = '\0';
                size_t nparse = parser->execute(data, len, true);

                if (parser->hasError()) {
                    close();
                    return nullptr;
                }
                len -= nparse;
                if (len == (int) buff_size) {
                    close();
                    return nullptr;
                }
            } while (!parser->isFinished());

            len -= 2;

            ZHOU_INFO(g_logger) << "content_len=" << client_parser.content_len;
            if(client_parser.content_len <= len) {
                body.append(data, client_parser.content_len);
                memmove(data, data + client_parser.content_len
                        , len - client_parser.content_len);
                len -= client_parser.content_len;
            } else {
                body.append(data, len);
                int left = client_parser.content_len - len;
                while(left > 0) {
                    int rt = read(data, left > (int)buff_size ? (int)buff_size : left);
                    if(rt <= 0) {
                        close();
                        return nullptr;
                    }
                    body.append(data, rt);
                    left -= rt;
                }
                len = 0;
            }
        } while (!client_parser.chunks_done);
        parser->getData()->setBody(body);
    } else {
        int64_t length = parser->getContentLength();
            //ZHOU_INFO(g_logger) << "content_len=" << client_parser.content_len;
            ZHOU_INFO(g_logger) << "content_len=" << length;
        if (length > 0) {
            std::string body;
            body.reserve(length);

            if (length >= offset) {
                body.append(data, offset);
            } else {
                body.append(data, length);
            }
            length -= offset;
            if (length > 0) {
                if (readFixSize(&body[body.size()], length) <= 0) {
                    return nullptr;
                }
            }
            parser->getData()->setBody(body);
        }
        //     body.resize(length);

        //     int len = 0;
        //     if (length >= offset) {
        //         memcpy(&body[0], data, offset);
        //         len = offset;
        //     } else {
        //         memcpy(&body[0], data, length);
        //         len = length;
        //     }
        //     length -= len;
        //     if (length > 0) {
        //         if (readFixSize(&body[len], length) <= 0) {
        //             close();
        //             return nullptr;
        //         }
        //     }
        //     parser->getData()->setBody(body);
        // }
    }
    return parser->getData();


}



}
}
