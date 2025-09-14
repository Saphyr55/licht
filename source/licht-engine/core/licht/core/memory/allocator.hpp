#pragma once

#include "licht/core/memory/memory.hpp"

namespace licht {

template <typename AllocatorType, typename ResourceType>
concept CAllocator = requires(AllocatorType t, ResourceType* r, usize n) {
    { t.allocate(n) } -> std::same_as<ResourceType*>;
    { t.deallocate(r, n) } -> std::same_as<void>;
};

template <typename ResourceType>
class DefaultAllocator {
public:
    template <typename U>
    struct rebind {
        using other = DefaultAllocator<U>;
    };

public:
    ResourceType* allocate(const usize n) {
        return static_cast<ResourceType*>(Memory::allocate(n * sizeof(ResourceType)));
    }

    void deallocate(ResourceType* resource, const usize n) {
        if (resource) {
            Memory::free(resource, n * sizeof(ResourceType));
        }
    }
};

}  // namespace licht
