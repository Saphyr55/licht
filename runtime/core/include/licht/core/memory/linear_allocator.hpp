#pragma once

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/allocator.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/typed_allocator.hpp"

namespace licht {

class LICHT_CORE_API LinearAllocator : public AlignedAllocator {
public:
    virtual void* allocate(size_t size, size_t alignment) override;

    virtual void deallocate(void* block, size_t size, size_t alignment) override;

public:
    void initialize(size_t size);

    void destroy();

    void reset();

    inline size_t get_size() const {
        return size_;
    }

    inline size_t get_offset() const {
        return offset_;
    }

    inline bool is_valid() const {
        return buffer_ != nullptr;
    }

public:
    LinearAllocator(size_t size);
    LinearAllocator();
    ~LinearAllocator();

private:
    uint8* buffer_;
    size_t size_;
    size_t offset_;
};

template <typename Type, MemoryOwnership ownership = MemoryOwnership::Owner>
class TypedLinearAllocator : public TypedAllocator<Type, LinearAllocator, ownership> {
public:
    using Base = TypedAllocator<Type, LinearAllocator, ownership>;

public:
    void reset() {
        Base::get_allocator().reset();
    }
};

}  // namespace licht