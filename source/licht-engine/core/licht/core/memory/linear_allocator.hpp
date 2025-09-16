#pragma once

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"

namespace licht {

class LICHT_CORE_API LinearMemoryPool {
public:
    void* allocate(usize size, usize alignment) noexcept;

    void deallocate(void* block) noexcept;

    void initialize(usize size) noexcept;

    void destroy() noexcept;

    void reset();

public:
    explicit LinearMemoryPool(usize size);
    LinearMemoryPool();
    ~LinearMemoryPool() noexcept;

private:
    usize size_;
    usize offset_;
    uint8* buffer_;
};

template <typename ElementType>
class LinearAllocator {
public:
    template<typename OtherElementType>
    struct rebind {
        using other = LinearAllocator<OtherElementType>;
    };

public:
    ElementType* allocate(const usize n) noexcept {
        void* element = memory_pool_->allocate(n * sizeof(ElementType), alignof(ElementType));
        return static_cast<ElementType*>(element);
    }
    
    void deallocate(ElementType* /* element */, usize /* n */) noexcept {
    }

public:
    explicit LinearAllocator(LinearMemoryPool* memory_pool)
        : memory_pool_(memory_pool) {}

private:
    LinearMemoryPool* memory_pool_;
};

}  //namespace licht