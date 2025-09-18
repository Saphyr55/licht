#pragma once

#include "licht/core/defines.hpp"

namespace licht {

class AlignedAllocator {
public:
    virtual void* allocate(usize size, usize alignment) = 0;
    virtual void deallocate(void* memory, usize size, usize alignment) = 0;
};

class Allocator {
public:
    virtual void* allocate(usize size) = 0;
    virtual void deallocate(void* memory, usize size) = 0;
};

}  //namespace licht