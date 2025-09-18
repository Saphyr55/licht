#pragma once

#include <type_traits>

#include "licht/core/memory/concepts.hpp"
#include "licht/core/memory/memory.hpp"

namespace licht {

template <typename ElementType,
          CAlignedAllocator<void> AllocatorType,
          MemoryOwnership ownership = MemoryOwnership::Owner>
class TypedAllocator {
public:
    template <typename OtherResourceType,
              CAlignedAllocator<void> OtherAllocatorType = AllocatorType,
              MemoryOwnership OtherOwnership = ownership>
    struct rebind {
        using other = TypedAllocator<OtherResourceType, OtherAllocatorType, OtherOwnership>;
    };

    using MemoryAllocatorType = std::conditional_t<
        ownership == MemoryOwnership::Owner,
        AllocatorType,
        AllocatorType*>;

public:
    ElementType* allocate(usize n)
        requires(ownership == MemoryOwnership::Owner)
    {
        return static_cast<ElementType*>(
            allocator_.allocate(n * sizeof(ElementType), alignof(ElementType)));
    }

    ElementType* allocate(usize n)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        return static_cast<ElementType*>(
            allocator_->allocate(n * sizeof(ElementType), alignof(ElementType)));
    }

    void deallocate(ElementType* element, usize n)
        requires(ownership == MemoryOwnership::Owner)
    {
        allocator_.deallocate(element, n * sizeof(ElementType), alignof(ElementType));
    }

    void deallocate(ElementType* element, usize n)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        allocator_->deallocate(element, n * sizeof(ElementType), alignof(ElementType));
    }

    ElementType* allocate(usize n, usize alignment)
        requires(ownership == MemoryOwnership::Owner)
    {
        return static_cast<ElementType*>(
            allocator_.allocate(n * sizeof(ElementType), alignment));
    }

    ElementType* allocate(usize n, usize alignment)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        return static_cast<ElementType*>(
            allocator_->allocate(n * sizeof(ElementType), alignment));
    }

    void deallocate(ElementType* element, usize n, usize alignment)
        requires(ownership == MemoryOwnership::Owner)
    {
        allocator_.deallocate(element, n * sizeof(ElementType), alignment);
    }

    void deallocate(ElementType* element, usize n, usize alignment)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        allocator_->deallocate(element, n * sizeof(ElementType), alignment);
    }

    bool is_valid() const
        requires(ownership == MemoryOwnership::NonOwner)
    {
        return allocator_ != nullptr;
    }

    bool is_valid() const
        requires(ownership == MemoryOwnership::Owner)
    {
        return true;
    }

    bool is_owner() const {
        return ownership == MemoryOwnership::Owner;
    }

    void set_allocator(AllocatorType& allocator) {
        if constexpr (ownership == MemoryOwnership::Owner) {
            allocator_ = allocator;
        } else {
            allocator_ = &allocator;
        }
    }

    AllocatorType& get_allocator() {
        if constexpr (ownership == MemoryOwnership::Owner) {
            return allocator_;
        } else {
            return *allocator_;
        }
    }

    const AllocatorType& get_allocator() const {
        if constexpr (ownership == MemoryOwnership::Owner) {
            return allocator_;
        } else {
            return *allocator_;
        }
    }

public:
    TypedAllocator() = default;

    TypedAllocator(AllocatorType* memory)
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(memory) {}

    TypedAllocator(AllocatorType& memory)
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(&memory) {}

    TypedAllocator(usize size)
        requires(ownership == MemoryOwnership::Owner)
        : allocator_(size) {}

    ~TypedAllocator() = default;

    TypedAllocator(const TypedAllocator& other)
        requires(ownership == MemoryOwnership::Owner)
        : allocator_(other.allocator_) {}

    TypedAllocator(const TypedAllocator& other)
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(other.allocator_) {}

    TypedAllocator(TypedAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::Owner)
        : allocator_(std::move(other.allocator_)) {}

    TypedAllocator(TypedAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(other.allocator_) {
        other.allocator_ = nullptr;
    }

    TypedAllocator& operator=(const TypedAllocator& other) {
        if (this != &other) {
            if constexpr (ownership == MemoryOwnership::Owner) {
                allocator_ = other.allocator_;
            } else {
                allocator_ = other.allocator_;
            }
        }
        return *this;
    }

    TypedAllocator& operator=(TypedAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::Owner)
    {
        if (this != &other) {
            allocator_ = std::move(other.allocator_);
        }
        return *this;
    }

    TypedAllocator& operator=(TypedAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::NonOwner)
    {
        if (this != &other) {
            allocator_ = other.allocator_;
            other.allocator_ = nullptr;
        }
        return *this;
    }

    bool operator==(const TypedAllocator& other) const {
        if constexpr (ownership == MemoryOwnership::Owner) {
            return &allocator_ == &other.allocator_;
        } else {
            return allocator_ == other.allocator_;
        }
    }

    bool operator!=(const TypedAllocator& other) const {
        return !(*this == other);
    }

protected:
    MemoryAllocatorType allocator_;
};

}  //namespace licht