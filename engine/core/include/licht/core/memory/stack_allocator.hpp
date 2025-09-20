#pragma once

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/typed_allocator.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/allocator.hpp"

namespace licht {

class LICHT_CORE_API StackAllocator : public AlignedAllocator {
public:
    virtual void* allocate(size_t size, size_t alignment) override;
    virtual void deallocate(void* memory, size_t size, size_t alignment) override;

public:
    inline void reset() {
        head_ = 0;
    }

    inline void pop_head() {
        pop(head_);
    }

    void pop(size_t marker) {
        LCHECK_MSG(marker <= size_, "Marker is out of bounds.");
        head_ = marker - 1;
    }   

    size_t get_head() const {
        return head_;
    }

    size_t size() const {
        return size_;
    }

private:
    void initialize(size_t size);
    void destroy();

public:
    explicit StackAllocator(size_t size);
    ~StackAllocator();

private:
    size_t size_;
    size_t head_;
    uint8* buffer_; 
};

template<typename Type, MemoryOwnership ownership = MemoryOwnership::Owner>
class TypedStackAllocator : public TypedAllocator<Type, StackAllocator, ownership> {
public:
    using Base = TypedAllocator<Type, StackAllocator, ownership>;

public:
    void reset() {
        Base::get_allocator().reset();
    }
    
    void pop_head() {
        Base::get_allocator().pop_head();
    }
    
    void pop(size_t marker) {
        Base::get_allocator().pop(marker);
    }

};

}