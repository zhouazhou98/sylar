#ifndef __ZHOU_HTTP_RESPONSE_H__
#define __ZHOU_HTTP_RESPONSE_H__

#include <memory>

namespace zhou {
namespace http {

class HttpResponse {
public:
    typedef std::shared_ptr<HttpResponse> ptr;

public:

private:
    HttpMethod m_method;
};

}
}

#endif // !__ZHOU_HTTP_RESPONSE_H__
