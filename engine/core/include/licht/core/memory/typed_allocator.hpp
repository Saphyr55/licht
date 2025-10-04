#pragma once

#include <type_traits>

#include "licht/core/memory/concepts.hpp"
#include "licht/core/memory/memory.hpp"

namespace licht {

template <typename ElementType,
          CAlignedAllocator AllocatorType,
          MemoryOwnership ownership = MemoryOwnership::Owner>
class TypedAllocator {
public:
    template <typename OtherResourceType,
              CAlignedAllocator OtherAllocatorType = AllocatorType,
              MemoryOwnership OtherOwnership = ownership>
    struct rebind {
        using other = TypedAllocator<OtherResourceType, OtherAllocatorType, OtherOwnership>;
    };

    using MemoryAllocatorType = std::conditional_t<
        ownership == MemoryOwnership::Owner,
        AllocatorType,
        AllocatorType*>;

public:
    constexpr ElementType* allocate(size_t n)
        requires(ownership == MemoryOwnership::Owner)
    {
        return static_cast<ElementType*>(
            allocator_.allocate(n * sizeof(ElementType), alignof(ElementType)));
    }

    constexpr ElementType* allocate(size_t n)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        return static_cast<ElementType*>(
            allocator_->allocate(n * sizeof(ElementType), alignof(ElementType)));
    }

    constexpr void deallocate(ElementType* element, size_t n)
        requires(ownership == MemoryOwnership::Owner)
    {
        allocator_.deallocate(element, n * sizeof(ElementType), alignof(ElementType));
    }

    constexpr void deallocate(ElementType* element, size_t n)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        allocator_->deallocate(element, n * sizeof(ElementType), alignof(ElementType));
    }

    constexpr ElementType* allocate(size_t n, size_t alignment)
        requires(ownership == MemoryOwnership::Owner)
    {
        return static_cast<ElementType*>(
            allocator_.allocate(n * sizeof(ElementType), alignment));
    }

    constexpr ElementType* allocate(size_t n, size_t alignment)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        return static_cast<ElementType*>(
            allocator_->allocate(n * sizeof(ElementType), alignment));
    }

    constexpr void deallocate(ElementType* element, size_t n, size_t alignment)
        requires(ownership == MemoryOwnership::Owner)
    {
        allocator_.deallocate(element, n * sizeof(ElementType), alignment);
    }

    constexpr void deallocate(ElementType* element, size_t n, size_t alignment)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        allocator_->deallocate(element, n * sizeof(ElementType), alignment);
    }

    constexpr bool is_valid() const
        requires(ownership == MemoryOwnership::NonOwner)
    {
        return allocator_ != nullptr;
    }

    constexpr bool is_valid() const
        requires(ownership == MemoryOwnership::Owner)
    {
        return true;
    }

    constexpr bool is_owner() const {
        return ownership == MemoryOwnership::Owner;
    }

    constexpr void set_allocator(AllocatorType& allocator) {
        if constexpr (ownership == MemoryOwnership::Owner) {
            allocator_ = allocator;
        } else {
            allocator_ = &allocator;
        }
    }

protected:
    constexpr AllocatorType& get_allocator() {
        if constexpr (ownership == MemoryOwnership::Owner) {
            return allocator_;
        } else {
            return *allocator_;
        }
    }

public:
    constexpr TypedAllocator() = default;

    constexpr TypedAllocator(AllocatorType* memory)
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(memory) {}

    constexpr TypedAllocator(AllocatorType& memory)
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(&memory) {}

    constexpr TypedAllocator(size_t size)
        requires(ownership == MemoryOwnership::Owner)
        : allocator_(size) {}

    constexpr ~TypedAllocator() = default;

    constexpr TypedAllocator(const TypedAllocator& other)
        requires(ownership == MemoryOwnership::Owner)
        : allocator_(other.allocator_) {}

    constexpr TypedAllocator(const TypedAllocator& other)
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(other.allocator_) {}

    constexpr TypedAllocator(TypedAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::Owner)
        : allocator_(std::move(other.allocator_)) {}

    constexpr TypedAllocator(TypedAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(other.allocator_) {
        other.allocator_ = nullptr;
    }

    constexpr TypedAllocator& operator=(const TypedAllocator& other) {
        if (this != &other) {
            if constexpr (ownership == MemoryOwnership::Owner) {
                allocator_ = other.allocator_;
            } else {
                allocator_ = other.allocator_;
            }
        }
        return *this;
    }

    constexpr TypedAllocator& operator=(TypedAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::Owner)
    {
        if (this != &other) {
            allocator_ = std::move(other.allocator_);
        }
        return *this;
    }

    constexpr TypedAllocator& operator=(TypedAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::NonOwner)
    {
        if (this != &other) {
            allocator_ = other.allocator_;
            other.allocator_ = nullptr;
        }
        return *this;
    }

    constexpr bool operator==(const TypedAllocator& other) const {
        if constexpr (ownership == MemoryOwnership::Owner) {
            return &allocator_ == &other.allocator_;
        } else {
            return allocator_ == other.allocator_;
        }
    }

    constexpr bool operator!=(const TypedAllocator& other) const {
        return !(*this == other);
    }

protected:
    MemoryAllocatorType allocator_;
};

}  //namespace licht