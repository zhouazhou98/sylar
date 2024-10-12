#include <zhou/zhou.h>
#include <iostream>

void test_request() {
    zhou::http::HttpRequest::ptr req(new zhou::http::HttpRequest);
    req->setHeader("X-X", "zhou");
    req->setHeader("host", "www.zhou.top");
    req->setBody("hello zhou!");
    req->dump(std::cout) << std::endl;
}

void test_response() {
    zhou::http::HttpResponse::ptr res(new zhou::http::HttpResponse);
    res->setHeader("X-X", "zhou");
    res->setBody("hello zhou!");
    res->setStatus((zhou::http::HttpStatus)200);
    res->setClose(false);
    res->dump(std::cout) << std::endl;
}

int main() {
    test_request();
    std::cout << "---------------------------------" << std::endl;
    test_response();
    return 0;
}

