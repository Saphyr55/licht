#include "linear_allocator.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"

#include <memory>

namespace licht {

void* LinearMemoryPool::allocate(const usize size, const usize alignment) noexcept {
    usize space = size_ - offset_;

    void* block = buffer_ + offset_;
    void* aligned = std::align(alignment, size, block, space);
    
    if (!aligned || static_cast<uint8*>(aligned) + size > buffer_ + size_) {
        return nullptr;  // Not enough space.
    }

    offset_ = static_cast<uint8*>(aligned) - buffer_ + size;

    return aligned;
}

void LinearMemoryPool::deallocate(void* block) noexcept {
    
}

void LinearMemoryPool::initialize(usize size) noexcept {
    size_ = size;
    if (buffer_) {
        destroy();
    }
    buffer_ = static_cast<uint8*>(Memory::allocate(size_));
    if (buffer_) {
        Memory::write(buffer_, 0, size_);
    }
}

void LinearMemoryPool::destroy() noexcept {
    if (buffer_) {
        return;
    }

    Memory::free(buffer_, size_);
    buffer_ = nullptr;
    size_ = 0;
    offset_ = 0;
}

void LinearMemoryPool::reset() {
    offset_ = 0;
}

LinearMemoryPool::LinearMemoryPool(usize size)
    : size_(32)
    , offset_(0)
    , buffer_(nullptr) {
    initialize(size);
}

LinearMemoryPool::LinearMemoryPool()
    : size_(32)
    , offset_(0)
    , buffer_(nullptr) {
    initialize(size_);
}

LinearMemoryPool::~LinearMemoryPool() noexcept {
    if (buffer_) {
        destroy();
    }
}

}  //namespace licht