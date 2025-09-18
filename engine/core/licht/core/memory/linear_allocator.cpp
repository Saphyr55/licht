#include "licht/core/memory/linear_allocator.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"

namespace licht {

void* LinearAllocator::allocate(const usize size, usize alignment) {
    usize space = size_ - offset_;

    void* block = buffer_ + offset_;
    void* aligned = Memory::align(block, alignment);

    if (!aligned || static_cast<uint8*>(aligned) + size > buffer_ + size_) {
        return nullptr;  // Not enough space.
    }

    offset_ = static_cast<uint8*>(aligned) - buffer_ + size;

    return aligned;
}

void LinearAllocator::deallocate(void* /* block */, usize /* size */, usize /* alignment */) {
    // Linear allocator does not support deallocation of individual blocks.
    // Deallocation is done by resetting the entire allocator.
}

void LinearAllocator::initialize(usize size) {
    size_ = size;
    if (buffer_) {
        destroy();
    }
    buffer_ = static_cast<uint8*>(Memory::allocate(size_));
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

LinearAllocator::LinearAllocator(usize size)
    : size_(32)
    , offset_(0)
    , buffer_(nullptr) {
    initialize(size);
}

LinearAllocator::LinearAllocator()
    : size_(32)
    , offset_(0)
    , buffer_(nullptr) {
    initialize(size_);
}

LinearAllocator::~LinearAllocator() {
    if (buffer_) {
        destroy();
    }
}

}  //namespace licht