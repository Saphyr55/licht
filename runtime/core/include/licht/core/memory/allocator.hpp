#pragma once

namespace licht {

struct NoAllocationOnConstructionPolicy {};

class Allocator {
public:
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* memory, size_t size) = 0;
};

class AlignedAllocator {
public:
    virtual void* allocate(size_t size, size_t alignment) = 0;
    virtual void deallocate(void* memory, size_t size, size_t alignment) = 0;
};

}  //namespace licht
