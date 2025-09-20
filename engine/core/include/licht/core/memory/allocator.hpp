#pragma once

namespace licht {

class AlignedAllocator {
public:
    virtual void* allocate(size_t size, size_t alignment) = 0;
    virtual void deallocate(void* memory, size_t size, size_t alignment) = 0;
};

class Allocator {
public:
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* memory, size_t size) = 0;
};

}  //namespace licht