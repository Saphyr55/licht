#pragma once

#include <atomic>

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/default_allocator.hpp"
#include "licht/core/memory/deleter.hpp"
#include "licht/core/memory/memory.hpp"

namespace licht {

class LICHT_CORE_API ReferenceCounter {
public:
    using RefCountType = std::atomic_int32_t;

public:
    ReferenceCounter() = default;
    virtual ~ReferenceCounter() = default;

    virtual void destroy_resource() = 0;

    int32 get_shared_reference_count() const {
        return shared_reference_count_.load(std::memory_order_relaxed);
    }

    bool is_unique() const {
        return get_shared_reference_count() == 1;
    }

    void add_shared_reference() {
        shared_reference_count_.fetch_add(1, std::memory_order_relaxed);
    }

    void release_shared_reference() {
        int32 prev_count = shared_reference_count_.fetch_sub(1, std::memory_order_relaxed);

        if (prev_count == 1) {
            // Ensure destruction and deletion are synchronized.
            std::atomic_thread_fence(std::memory_order_acquire);

            destroy_resource();
            ldelete(DefaultAllocator::get_instance(), this);
        }
    }

protected:
    RefCountType shared_reference_count_ = 1;
};

template <typename ResourceType, typename DeleterType>
class ReferenceCounterWithDeleter : private DeleterDelegate<DeleterType>,
                                    public ReferenceCounter {
public:
    virtual void destroy_resource() override {
        if (resource_) {
            this->invoke(resource_);
            resource_ = nullptr;
        }
    }

    ReferenceCounterWithDeleter(ResourceType* resource, const DeleterType& deleter)
        : DeleterDelegate<DeleterType>(deleter)
        , resource_(resource) {
        LCHECK(resource_);
    }

    ReferenceCounterWithDeleter(const ReferenceCounterWithDeleter&) = delete;
    ReferenceCounterWithDeleter& operator=(const ReferenceCounterWithDeleter&) = delete;

    ReferenceCounterWithDeleter(ReferenceCounterWithDeleter&& other) noexcept
        : DeleterDelegate<DeleterType>(std::move(other))
        , resource_(std::exchange(other.resource_, nullptr)) {}

    ReferenceCounterWithDeleter& operator=(ReferenceCounterWithDeleter&& other) noexcept {
        if (this != &other) {
            destroy_resource();

            DeleterDelegate<DeleterType>::operator=(std::move(other));
            resource_ = std::exchange(other.resource_, nullptr);
        }
        return *this;
    }

private:
    ResourceType* resource_ = nullptr;
};

template <typename ResourceType, typename DeleterType>
inline ReferenceCounter* new_reference_counter_with_deleter(ResourceType* resource, const DeleterType& deleter) noexcept {
    using RefCounter = ReferenceCounterWithDeleter<ResourceType, DeleterType>;
    return lnew<RefCounter>(DefaultAllocator::get_instance(), resource, deleter);
}

template <typename ResourceType>
inline ReferenceCounter* new_default_reference_counter(ResourceType* resource) noexcept {
    using DeleterType = DefaultDeleter<ResourceType>;
    return new_reference_counter_with_deleter(resource, DeleterType(DefaultAllocator::get_instance()));
}

}  // namespace licht