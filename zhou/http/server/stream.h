#ifndef __ZHOU_STREAM_H__
#define __ZHOU_STREAM_H__

#include <memory>

// 解决粘包问题

namespace zhou {

class Stream {
public:
    typedef std::shared_ptr<Stream> ptr;
    
    virtual ~Stream();

public:
    virtual int read(void * buffer, size_t length) = 0;
    virtual int readFixSize(void * buffer, size_t length);

    virtual int write(const void * buffer, size_t length) = 0;
    virtual int writeFixSize(const void * buffer, size_t length);

    virtual void close() = 0;

};


}

#endif // ! __ZHOU_STREAM_H__
