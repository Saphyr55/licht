#include "buffered_allocator.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/linear_allocator.hpp"

namespace licht {

void* BufferedAllocator::allocate(size_t size, size_t alignment) {
    LinearAllocator& linear_allocator = get_allocator(current_allocator_);
    return linear_allocator.allocate(size, alignment);
}

void BufferedAllocator::initialize(size_t size) {
    for (int32 i = 0; i < BufferedCount; ++i) {
        get_allocator(i).initialize(size);
    }
}

void BufferedAllocator::destroy() {
    for (int32 i = 0; i < BufferedCount; i++) {
        get_allocator(i).destroy();
    }
}

void BufferedAllocator::reset() {
    get_allocator(current_allocator_).reset();
}

LinearAllocator& BufferedAllocator::get_allocator(int32 index) {
    LCHECK_MSG(index < BufferedCount, "Index out of range");
    return allocators_[index % BufferedCount];
}

}  //namespace licht