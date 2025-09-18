#include "linear_allocator.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"

#include <memory>

namespace licht {

void* LinearMemoryAllocator::allocate(const usize size, usize alignment) {
    usize space = size_ - offset_;

    void* block = buffer_ + offset_;
    void* aligned = Memory::align(block, alignment);

    if (!aligned || static_cast<uint8*>(aligned) + size > buffer_ + size_) {
        return nullptr;  // Not enough space.
    }

    offset_ = static_cast<uint8*>(aligned) - buffer_ + size;

    return aligned;
}

void LinearMemoryAllocator::deallocate(void* block) {
}

void LinearMemoryAllocator::initialize(usize size) {
    size_ = size;
    if (buffer_) {
        destroy();
    }
    buffer_ = static_cast<uint8*>(Memory::allocate(size_));
    if (buffer_) {
        Memory::write(buffer_, 0, size_);
    }
}

void LinearMemoryAllocator::destroy() {
    if (buffer_) {
        return;
    }

    Memory::free(buffer_, size_);
    buffer_ = nullptr;
    size_ = 0;
    offset_ = 0;
}

void LinearMemoryAllocator::reset() {
    offset_ = 0;
}

LinearMemoryAllocator::LinearMemoryAllocator(usize size)
    : size_(32)
    , offset_(0)
    , buffer_(nullptr) {
    initialize(size);
}

LinearMemoryAllocator::LinearMemoryAllocator()
    : size_(32)
    , offset_(0)
    , buffer_(nullptr) {
    initialize(size_);
}

LinearMemoryAllocator::~LinearMemoryAllocator() {
    if (buffer_) {
        destroy();
    }
}

}  //namespace licht