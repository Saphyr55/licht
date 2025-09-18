#pragma once

#include "licht/core/defines.hpp"

namespace licht {

template<typename ResourceType>
class StackAllocator {
    LICHT_REBIND_TEMPLATE(StackAllocator, OtherResourceType)

    void* allocate(usize size);
    void deallocate(void* memory, usize size);
    
    void* allocate(usize size, usize alignment);
    void deallocate(void* memory, usize size, usize alignment);

public:
    StackAllocator();
    ~StackAllocator();

private:
    usize marker_;
    uint8* resources_; 
};

}