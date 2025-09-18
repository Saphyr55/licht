#pragma once

#include "licht/core/memory/memory.hpp"

namespace licht {

template <typename AllocatorType, typename ResourceType>
concept CAllocator = requires(AllocatorType t, ResourceType* r, usize n, usize alignment) {
    { t.allocate(n) } -> std::same_as<ResourceType*>;
    { t.deallocate(r, n) } -> std::same_as<void>;
    { t.allocate(n, alignment) } -> std::same_as<ResourceType*>;
    { t.deallocate(r, n, alignment) } -> std::same_as<void>;
};

enum class MemoryOwnership {
    Owner,
    NonOwner
};

template <typename ResourceType>
class DefaultAllocator {
    LICHT_REBIND_TEMPLATE(DefaultAllocator, OtherResourceType)

public:
    ResourceType* allocate(usize count, usize alignment) {
        const usize size = count * sizeof(ResourceType);
        void* memory = Memory::allocate(size);
        return Memory::align<ResourceType>(memory, alignment);
    }

    ResourceType* allocate(usize count) {
        return static_cast<ResourceType*>(Memory::allocate(count * sizeof(ResourceType)));
    }

    void deallocate(ResourceType* ptr, usize count, usize alignment) {
        const usize size = count * sizeof(ResourceType);
        Memory::free(ptr, size);
    }
    void deallocate(ResourceType* ptr, usize count) {
        deallocate(ptr, count, alignof(ResourceType));
    }
};

}  // namespace licht
