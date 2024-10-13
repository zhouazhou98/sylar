#include "http_parser.h"

namespace zhou {
namespace http {






class HttpRequest;
std::ostream & operator<< (std::ostream & os, HttpRequest & req) {
    return req.dump(os);
}

class HttpResponse;
std::ostream & operator<< (std::ostream & os, HttpResponse & res) {
    return res.dump(os);
}
}   // ! http namespace
}   // ! zhou namespace