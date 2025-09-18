#pragma once

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/typed_allocator.hpp"
#include "licht/core/memory/allocator.hpp"

namespace licht {

class LICHT_CORE_API LinearAllocator : public AlignedAllocator {
public:
    virtual void* allocate(usize size, usize alignment) override;

    virtual void deallocate(void* block, usize size, usize alignment) override;

public:
    void initialize(usize size);

    void destroy();

    void reset();

public:
    explicit LinearAllocator(usize size);
    LinearAllocator();
    ~LinearAllocator();

private:
    usize size_;
    usize offset_;
    uint8* buffer_;
};

template<typename Type, MemoryOwnership ownership = MemoryOwnership::Owner>
class TypedLinearAllocator : public TypedAllocator<Type, LinearAllocator, ownership> {
public:
    using Base = TypedAllocator<Type, LinearAllocator, ownership>;

public:
    void reset() {
        Base::get_allocator().reset();
    }
};

}  // namespace licht