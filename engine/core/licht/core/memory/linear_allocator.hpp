#pragma once

#include <type_traits>

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/allocator.hpp"

namespace licht {

class LICHT_CORE_API LinearMemoryAllocator {
public:
    void* allocate(usize size, usize alignment);

    void deallocate(void* block);

    void initialize(usize size);

    void destroy();

    void reset();

public:
    explicit LinearMemoryAllocator(usize size);
    LinearMemoryAllocator();
    ~LinearMemoryAllocator();

private:
    usize size_;
    usize offset_;
    uint8* buffer_;
};

template <typename ElementType,
          MemoryOwnership ownership = MemoryOwnership::Owner>
class LinearAllocator {
    LICHT_REBIND_TEMPLATE(LinearAllocator, OtherElementType)

public:
    using LinearMemoryAllocatorType =
        std::conditional_t<ownership == MemoryOwnership::Owner,
                           LinearMemoryAllocator,
                           LinearMemoryAllocator*>;


public:
    ElementType* allocate(const usize n) {
        if constexpr (ownership == MemoryOwnership::Owner) {
            return static_cast<ElementType*>(
                memory_.allocate(n * sizeof(ElementType), alignof(ElementType)));
        } else {
            return static_cast<ElementType*>(
                memory_->allocate(n * sizeof(ElementType), alignof(ElementType)));
        }
    }

    void deallocate(ElementType* /* element */, usize /* n */) {}

    ElementType* allocate(const usize n, const usize alignment) {
        if constexpr (ownership == MemoryOwnership::Owner) {
            return static_cast<ElementType*>(
                memory_.allocate(n * sizeof(ElementType), alignment));
        } else {
            return static_cast<ElementType*>(
                memory_->allocate(n * sizeof(ElementType), alignment));
        }
    }

    void deallocate(ElementType* /* element */,
                    usize /* n */,
                    usize /* alignment */) {}

    void reset() {
        if constexpr (ownership == MemoryOwnership::Owner) {
            memory_.reset();
        } else {
            if (memory_) {
                memory_->reset();
            }
        }
    }

    bool is_valid() const
        requires(ownership == MemoryOwnership::NonOwner)
    {
        return memory_ != nullptr;
    }

    bool is_valid() const
        requires(ownership == MemoryOwnership::Owner)
    {
        return true;
    }

    bool is_owner() const {
        return ownership == MemoryOwnership::Owner;
    }

    void set_memory_allocator(LinearMemoryAllocator* allocator)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        memory_ = allocator;
    }

public:
    LinearAllocator()
        requires(ownership == MemoryOwnership::NonOwner)
        : memory_(nullptr) {}

    LinearAllocator(LinearMemoryAllocator* memory)
        requires(ownership == MemoryOwnership::NonOwner)
        : memory_(memory) {}

    LinearAllocator(LinearMemoryAllocator& memory)
        requires(ownership == MemoryOwnership::NonOwner)
        : memory_(&memory) {}

    LinearAllocator(usize size)
        requires(ownership == MemoryOwnership::Owner)
        : memory_(size) {}

    ~LinearAllocator() = default;

    LinearAllocator(const LinearAllocator& other)
        requires(ownership == MemoryOwnership::Owner)
        : memory_(other.memory_) {}

    LinearAllocator(const LinearAllocator& other)
        requires(ownership == MemoryOwnership::NonOwner)
        : memory_(other.memory_) {}

    LinearAllocator(LinearAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::Owner)
        : memory_(std::move(other.memory_)) {}

    LinearAllocator(LinearAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::NonOwner)
        : memory_(other.memory_) {
        other.memory_ = nullptr;
    }

    LinearAllocator& operator=(const LinearAllocator& other) {
        if (this != &other) {
            if constexpr (ownership == MemoryOwnership::Owner) {
                memory_ = other.memory_;
            } else {
                memory_ = other.memory_;
            }
        }
        return *this;
    }

    LinearAllocator& operator=(LinearAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::Owner)
    {
        if (this != &other) {
            memory_ = std::move(other.memory_);
        }
        return *this;
    }

    LinearAllocator& operator=(LinearAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::NonOwner)
    {
        if (this != &other) {
            memory_ = other.memory_;
            other.memory_ = nullptr;
        }
        return *this;
    }

    bool operator==(const LinearAllocator& other) const {
        if constexpr (ownership == MemoryOwnership::Owner) {
            return &memory_ == &other.memory_;
        } else {
            return memory_ == other.memory_;
        }
    }

    bool operator!=(const LinearAllocator& other) const {
        return !(*this == other);
    }

    LinearMemoryAllocator& get_memory_allocator() {
        if constexpr (ownership == MemoryOwnership::Owner) {
            return memory_;
        } else {
            return *memory_;
        }
    }

    const LinearMemoryAllocator& get_memory_allocator() const {
        if constexpr (ownership == MemoryOwnership::Owner) {
            return memory_;
        } else {
            return *memory_;
        }
    }

private:
    LinearMemoryAllocatorType memory_;
};

}  // namespace licht