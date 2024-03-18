
#include <malloc.h>
#include "stack_allocator.h"

namespace zhou {    // __ZHOU_STACK_ALLOCATOR_H__

void * MallocStackAllocator::Alloc(size_t size) {
    return malloc(size);
}

// 这里对于 malloc 分配的堆空间释放只需要 ptr， 为了后续可扩展性 （实现对 mmap 的支持）， 这里预留 size 参数
void MallocStackAllocator::Dealloc(void * ptr, size_t size) {
    return free(ptr);
}


}   // ! __ZHOU_STACK_ALLOCATOR_H__

