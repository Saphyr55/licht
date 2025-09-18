#pragma once

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/typed_allocator.hpp"
#include "memory.hpp"

namespace licht {

class LICHT_CORE_API StackAllocator {
public:
    void* allocate(usize size);
    void deallocate(void* memory, usize size);

    void* allocate(usize size, usize alignment);
    void deallocate(void* memory, usize size, usize alignment);

    void initialize(usize size);
    void reset();

    void pop_head();

    usize head() const {
        return head_;
    }

    usize size() const {
        return size_;
    }

public:
    explicit StackAllocator(usize size);
    ~StackAllocator();

private:
    usize size_;
    usize head_;
    void* data_; 
};

template<typename Type, MemoryOwnership ownership = MemoryOwnership::Owner>
class TypedStackAllocator : public TypedAllocator<Type, StackAllocator, ownership> {
};

}