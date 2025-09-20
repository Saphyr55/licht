#include "licht/core/memory/stack_allocator.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"

namespace licht {

StackAllocator::StackAllocator(size_t size)
    : size_(size), head_(0), buffer_(nullptr) {
    initialize(size);
}

StackAllocator::~StackAllocator() {
    destroy();
}

void StackAllocator::initialize(size_t size) {
    buffer_ = Memory::allocate(size);
    size_ = size;
    head_ = 0;
}

void StackAllocator::destroy() {
    Memory::free(buffer_, size_);
    buffer_ = nullptr;
    size_ = 0;
    head_ = 0;
}

void* StackAllocator::allocate(size_t size, size_t alignment) {
    uint8* current_address = buffer_ + head_;
    uint8* aligned_address = Memory::align(current_address, alignment);

    size_t padding = aligned_address - current_address;
    size_t total_size = size + padding;

    if (head_ + total_size > size_) {
        // Not enough space.
        return nullptr;
    }

    head_ += total_size;

    return aligned_address;
}

void StackAllocator::deallocate(void* /* memory */, size_t /* size */, size_t /* alignment */) {
    // Stack allocator does not support deallocation of individual blocks.
    // Deallocation is done by resetting the entire allocator.
}

}  //namespace licht