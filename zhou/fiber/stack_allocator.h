#ifndef __ZHOU_STACK_ALLOCATOR_H__
#define __ZHOU_STACK_ALLOCATOR_H__

#include <malloc.h>
#include "fiber.h"

namespace zhou {    // __ZHOU_STACK_ALLOCATOR_H__

class MallocStackAllocator {
public:
    static void * Alloc(size_t size);
    static void Dealloc(void * ptr, size_t size);
};


}   // ! __ZHOU_STACK_ALLOCATOR_H__

#endif  // ! __ZHOU_STACK_ALLOCATOR_H__