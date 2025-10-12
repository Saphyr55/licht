#include "licht/core/memory/linear_allocator.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"

namespace licht {

void* LinearAllocator::allocate(const size_t size, size_t alignment) {
    size_t space = size_ - offset_;
    uint8* block = buffer_ + offset_;
    uint8* aligned_block = Memory::align(block, alignment);

    if (!aligned_block || aligned_block + size > buffer_ + size_) {
        // Not enough space.
        return nullptr;
    }

    offset_ = aligned_block - buffer_ + size;

    return aligned_block;
}

void LinearAllocator::deallocate(void* /* block */, size_t /* size */, size_t /* alignment */) {
    // Linear allocator does not support deallocation of individual blocks.
    // Deallocation is done by resetting the entire allocator.
}

void LinearAllocator::initialize(size_t size) {
    size_ = size;
    if (buffer_) {
        destroy();
    }
    buffer_ = Memory::allocate(size_);
    if (buffer_) {
        Memory::write(buffer_, 0, size_);
    }
}

void LinearAllocator::destroy() {
    if (buffer_) {
        return;
    }

    Memory::free(buffer_, size_);
    buffer_ = nullptr;
    size_ = 0;
    offset_ = 0;
}

void LinearAllocator::reset() {
    offset_ = 0;
}

LinearAllocator::LinearAllocator(size_t size)
    : LinearAllocator() {
    initialize(size);
}

LinearAllocator::LinearAllocator()
    : size_(32)
    , offset_(0)
    , buffer_(nullptr) {
}

LinearAllocator::~LinearAllocator() {
    if (buffer_) {
        destroy();
    }
}

}  //namespace licht