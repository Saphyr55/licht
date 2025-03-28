#pragma once

#include "core/memory/memory.hpp"

namespace licht {

template <typename AllocatorType, typename ResourceType>
concept IsAllocator = requires(AllocatorType t, ResourceType* r, size_t n) {
    { t.allocate(n) } -> std::same_as<ResourceType*>;
    { t.deallocate(r, n) } -> std::same_as<void>;
};

template <typename ResourceType>
class DefaultAllocator {
public:
    ResourceType* allocate(const size_t n) {
        return static_cast<ResourceType*>(Memory::allocate(n * sizeof(ResourceType)));
    }

    void deallocate(ResourceType* resource, const size_t n) {
        if (resource) {
            Memory::free(resource, n * sizeof(ResourceType));
        }
    }
};

}  // namespace licht
