#include "http_parser.h"

namespace zhou {
namespace http {


// bool CaseInsensitiveLess::operator() (const std::string & lhs, const std::string & rhs) const {
//     return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
// }




class HttpRequest;
std::ostream & operator<< (std::ostream & os, HttpRequest & req) {
    return req.dump(os);
}

// class HttpResponse;
// std::ostream & operator<< (std::ostream & os, HttpResponse & res) {
//     return res.dump(os);
// }
}   // ! http namespace
}   // ! zhou namespace