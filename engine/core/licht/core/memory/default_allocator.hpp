#pragma once

#include "allocator.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/typed_allocator.hpp"

namespace licht {

class LICHT_CORE_API DefaultAllocator : public Allocator, public AlignedAllocator {
public:
    virtual void* allocate(usize size, usize alignment) override {
        return Memory::allocate(size, alignment);
    }

    virtual void deallocate(void* memory, usize size, usize alignment) override{
        Memory::free(memory, size, alignment);
    }

    virtual void* allocate(usize size) override{
        return Memory::allocate(size);
    }

    virtual void deallocate(void* memory, usize size) override {
        Memory::free(memory, size);
    }
};

template<typename Type, MemoryOwnership ownership = MemoryOwnership::Owner>
using TypedDefaultAllocator = TypedAllocator<Type, DefaultAllocator, ownership>;

}  // namespace licht
