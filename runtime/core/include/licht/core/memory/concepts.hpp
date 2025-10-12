#pragma once

#include <concepts>

namespace licht {

template <typename AllocatorType>
concept CAllocator = requires(AllocatorType allocator, void* resource, size_t n) {
    { allocator.allocate(n) } -> std::same_as<void*>;
    { allocator.deallocate(resource, n) } -> std::same_as<void>;
};

template <typename AllocatorType>
concept CAlignedAllocator = requires(AllocatorType allocator, void* resource, size_t n, size_t alignment) {
    { allocator.allocate(n, alignment) } -> std::same_as<void*>;
    { allocator.deallocate(resource, n, alignment) } -> std::same_as<void>;
};

template <typename AllocatorType, typename ResourceType>
concept CTypedAllocator = requires(AllocatorType allocator, ResourceType* resource, size_t n) {
    { allocator.allocate(n) } -> std::same_as<ResourceType*>;
    { allocator.deallocate(resource, n) } -> std::same_as<void>;
};

template <typename AllocatorType, typename ResourceType>
concept CTypedAlignedAllocator = requires(AllocatorType allocator, ResourceType* resource, size_t n, size_t alignment) {
    { allocator.allocate(n, alignment) } -> std::same_as<ResourceType*>;
    { allocator.deallocate(resource, n, alignment) } -> std::same_as<void>;
};

}  //namespace licht
