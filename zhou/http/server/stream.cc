#include "stream.h"

// 解决粘包问题

namespace zhou {

Stream::~Stream() {}


int Stream::readFixSize(void * buffer, size_t length) {
    size_t offset = 0;
    size_t left = length;
    while (left > 0) {
        size_t len = read((char *) buffer + offset, left);
        if (len <= 0) {
            return len;
        }
        offset += len;
        left -= len;
    }
    return length;
}

int Stream::writeFixSize(const void * buffer, size_t length) {
    size_t offset = 0;
    size_t left = length;
    while (left > 0) {
        size_t len = write((const char *) buffer + offset, left);
        if (len <= 0) {
            return len;
        }
        offset += len;
        left -= len;
    }
    return length;
}


}
