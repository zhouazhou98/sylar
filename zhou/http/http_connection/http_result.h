#ifndef __ZHOU_HTTP_RESULT_H__
#define __ZHOU_HTTP_RESULT_H__

#include "zhou/http/http_parser/http_response/http_response.h"
#include <memory>

namespace zhou {
namespace http {

class HttpResult {
public:
    typedef std::shared_ptr<HttpResult> ptr;

    HttpResult(int result, HttpResponse::ptr response, const std::string & error);

public:
    enum class Error {
        // 正常
        OK = 0,
        // 非法URL
        INVALID_URL = 1,
        // 无法解析HOST
        INVALID_HOST = 2,
        // 连接失败
        CONNECT_FAIL = 3,
        // 连接被对端关闭
        SEND_CLOSE_BY_PEER = 4,
        // 发送请求产生Socket错误
        SEND_SOCKET_ERROR = 5,
        // 超时
        TIMEOUT = 6,
        // 创建Socket失败
        CREATE_SOCKET_ERROR = 7,
        // 从连接池中取连接失败
        POOL_GET_CONNECTION = 8,
        // 无效的连接
        POOL_INVALID_CONNECTION = 9,
    };

public:
    int getResult() const { return m_result; }
    HttpResponse::ptr getHttpResponse() const { return m_response; }
    const std::string & getError() const { return m_error; }

    void setResult(int result) { m_result = result; }
    void setHttpResponse(HttpResponse::ptr response) { m_response = response; }
    void setError(const std::string & error) { m_error = error; }

public:
    std::string toString() const;

private:
    int m_result;
    HttpResponse::ptr m_response;
    std::string m_error;
};


}
}


#endif // ! __ZHOU_HTTP_RESULT_H__
