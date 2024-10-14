#include "http_result.h"
#include <sstream>

namespace zhou {
namespace http {

HttpResult::HttpResult(int result, HttpResponse::ptr response, const std::string & error)
        : m_result(result), m_response(response), m_error(error) 
{
}

std::string HttpResult::toString() const {
    std::stringstream ss;
    ss << "[ HttpResult result = " << m_result
            << ", error = " << m_error
            << ", response = " << (m_response ? m_response->toString() : "nullptr")
            << "]";
    return ss.str();
}

}
}
