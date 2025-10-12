#pragma once

#include "licht/core/core_exports.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/typed_allocator.hpp"
#include "licht/core/memory/heap_allocator.hpp"

namespace licht {

class LICHT_CORE_API DefaultAllocator : public HeapAllocator {
public:
    static DefaultAllocator& get_instance();
};

template <typename Type, MemoryOwnership ownership = MemoryOwnership::Owner>
using TypedDefaultAllocator = TypedAllocator<Type, DefaultAllocator, ownership>;

}  // namespace licht
