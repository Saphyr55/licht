#pragma once

#include "licht/core/memory/allocator.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/typed_allocator.hpp"

namespace licht {

class HeapAllocator : public AlignedAllocator,
                      public Allocator {
public:
    inline virtual void* allocate(size_t size) override {
        return lmalloc(size);
    }

    inline virtual void deallocate(void* memory, size_t size) override {
        lfree(memory, size);
    }

    inline virtual void* allocate(size_t size, size_t alignment) override {
        return lalign_malloc(size, alignment);
    }

    inline virtual void deallocate(void* memory, size_t size, size_t alignment) override {
        lalign_free(memory, size, alignment);
    }

    HeapAllocator() = default;
    ~HeapAllocator() = default;
};

template <typename Type, MemoryOwnership ownership = MemoryOwnership::Owner>
using TypedHeapAllocator = TypedAllocator<Type, HeapAllocator, ownership>;

}  //namespace licht