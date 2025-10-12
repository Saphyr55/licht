#pragma once

#include "concepts.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/default_allocator.hpp"
#include "licht/core/memory/memory.hpp"

namespace licht {

template <typename DeleterType>
class DeleterDelegate {
public:
    template <typename ResourceType>
    void invoke(ResourceType* resource) noexcept {
        deleter_(resource);
    }

    explicit DeleterDelegate(const DeleterType& deleter)
        : deleter_(deleter) {}

private:
    DeleterType deleter_;
};

template <typename ResourceType, CAlignedAllocator AllocatorType>
class Deleter {
public:
    void operator()(ResourceType* resource) noexcept {
        destroy(resource);
    }

    void destroy(ResourceType* resource) noexcept {
        LCHECK(resource);
        ldelete(allocator_, resource);
    }

    Deleter(AllocatorType& allocator)
        : allocator_(allocator) {
    }

    virtual ~Deleter() = default;

private:
    AllocatorType& allocator_;
};

template <typename ResourceType>
using DefaultDeleter = Deleter<ResourceType, DefaultAllocator>;

template <typename ResourceType>
inline auto create_deleter(CAlignedAllocator auto& allocator) -> auto {
    return Deleter<ResourceType, decltype(allocator)>(allocator);
}

}  // namespace licht
