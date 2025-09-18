#pragma once

#include "concepts.hpp"
#include "licht/core/memory/memory.hpp"

namespace licht {

template <CAlignedAllocator<void> AllocatorType, MemoryOwnership ownership = MemoryOwnership::Owner>
class RawAllocator {
    using MemoryAllocatorType = std::conditional_t<
        ownership == MemoryOwnership::Owner,
        AllocatorType,
        AllocatorType*>;

public:
    void* allocate(size_t nbytes)
        requires(ownership == MemoryOwnership::Owner)
    {
        return allocator_.allocate(nbytes);
    }

    void* allocate(size_t nbytes)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        return allocator_->allocate(nbytes);
    }

    void deallocate(void* element, size_t nbytes)
        requires(ownership == MemoryOwnership::Owner)
    {
        allocator_.deallocate(element, nbytes);
    }

    void deallocate(void* element, size_t nbytes)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        allocator_->deallocate(element, nbytes);
    }

    void* allocate(size_t nbytes, size_t alignment)
        requires(ownership == MemoryOwnership::Owner)
    {
        return allocator_.allocate(nbytes, alignment);
    }

    void* allocate(size_t nbytes, size_t alignment)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        return allocator_.allocate(nbytes, alignment);
    }

    void deallocate(void* element, size_t nbytes, size_t alignment)
        requires(ownership == MemoryOwnership::Owner)
    {
        allocator_.deallocate(element, nbytes, alignment);
    }

    void deallocate(void* element, size_t nbytes, size_t alignment)
        requires(ownership == MemoryOwnership::NonOwner)
    {
        allocator_->deallocate(element, nbytes, alignment);
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
    RawAllocator() = default;

    RawAllocator(AllocatorType* memory)
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(memory) {}

    RawAllocator(AllocatorType& memory)
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(&memory) {}

    RawAllocator(size_t nbytes)
        requires(ownership == MemoryOwnership::Owner)
        : allocator_(nbytes) {}

    ~RawAllocator() = default;

    RawAllocator(const RawAllocator& other)
        requires(ownership == MemoryOwnership::Owner)
        : allocator_(other.allocator_) {}

    RawAllocator(const RawAllocator& other)
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(other.allocator_) {}

    RawAllocator(RawAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::Owner)
        : allocator_(std::move(other.allocator_)) {}

    RawAllocator(RawAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::NonOwner)
        : allocator_(other.allocator_) {
        other.allocator_ = nullptr;
    }

    RawAllocator& operator=(const RawAllocator& other) {
        if (this != &other) {
            if constexpr (ownership == MemoryOwnership::Owner) {
                allocator_ = other.allocator_;
            } else {
                allocator_ = other.allocator_;
            }
        }
        return *this;
    }

    RawAllocator& operator=(RawAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::Owner)
    {
        if (this != &other) {
            allocator_ = std::move(other.allocator_);
        }
        return *this;
    }

    RawAllocator& operator=(RawAllocator&& other) noexcept
        requires(ownership == MemoryOwnership::NonOwner)
    {
        if (this != &other) {
            allocator_ = other.allocator_;
            other.allocator_ = nullptr;
        }
        return *this;
    }

    bool operator==(const RawAllocator& other) const {
        if constexpr (ownership == MemoryOwnership::Owner) {
            return &allocator_ == &other.allocator_;
        } else {
            return allocator_ == other.allocator_;
        }
    }

    bool operator!=(const RawAllocator& other) const {
        return !(*this == other);
    }

private:
    MemoryAllocatorType allocator_;
};

}  //namespace licht