#include "stack_allocator.hpp"

namespace licht {

StackAllocator::StackAllocator(usize size) : size_(size), head_(0), data_(nullptr) {
    initialize(size);
}

StackAllocator::~StackAllocator() {
}

void StackAllocator::initialize(usize size) {

}

void* StackAllocator::allocate(usize size) {
    return nullptr;
}

void StackAllocator::deallocate(void* memory, usize size) {
}

void* StackAllocator::allocate(usize size, usize alignment) {
    return nullptr;
}

void StackAllocator::deallocate(void* memory, usize size, usize alignment) {
}

}  //namespace licht