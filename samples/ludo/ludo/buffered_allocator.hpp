#pragma once

#include "licht/core/containers/static_array.hpp"
#include "licht/core/memory/linear_allocator.hpp"

namespace licht {

class BufferedAllocator {
public:
    static constexpr uint8 BufferedCount = 2;

public:
    void* allocate(size_t size, size_t alignment);

    template <typename Type>
    Type* allocate(const size_t count = 1) {
        return static_cast<Type*>(allocate(sizeof(Type) * count, alignof(Type)));
    }

    void initialize(size_t size);

    void destroy();

    void reset();

    void swap_buffers() { current_allocator_ = (current_allocator_ + 1) % BufferedCount; }

    LinearAllocator& get_allocator(int32 index);

public:
    BufferedAllocator() = default;
    ~BufferedAllocator() = default;

private:
    StaticArray<LinearAllocator, BufferedCount> allocators_;
    uint8 current_allocator_ = 0;
};

}  //namespace licht