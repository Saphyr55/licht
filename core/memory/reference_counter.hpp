#pragma once

#include <atomic>
#include <memory>
#include <type_traits>

#include "core/core_exports.hpp"
#include "core/defines.hpp"
#include "core/memory/deleter.hpp"


namespace licht {

class LICHT_CORE_API ReferenceCounter {
public:
    using RefCountType = std::atomic_int32_t;

public:
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
        int32_t prev_count = shared_reference_count_.fetch_sub(1, std::memory_order_release);

        if (prev_count == 1) {
            // Ensure destruction and deletion are synchronized.
            std::atomic_thread_fence(std::memory_order_acquire);

            destroy_resource();

            delete this;
        }
    }

protected:
    RefCountType shared_reference_count_ = 1;
};

template <typename ResourceType, typename DeleterType>
class ReferenceCounterWithDeleter : private DeleterHolder<DeleterType>,
                                    public ReferenceCounter {
public:
    virtual void destroy_resource() override {
        if (resource_) {
            this->invoke_deleter(resource_);
            resource_ = nullptr;
        }
    }

    ReferenceCounterWithDeleter(ResourceType* resource, DeleterType&& deleter)
        : DeleterHolder<DeleterType>(std::move(deleter))
        , resource_(resource) {
        LCHECK(resource_ != nullptr);
    }

    ReferenceCounterWithDeleter(const ReferenceCounterWithDeleter&) = delete;
    ReferenceCounterWithDeleter& operator=(const ReferenceCounterWithDeleter&) = delete;

    ReferenceCounterWithDeleter(ReferenceCounterWithDeleter&& other) noexcept
        : DeleterHolder<DeleterType>(std::move(other))
        , resource_(std::exchange(other.resource_, nullptr)) {}

    ReferenceCounterWithDeleter& operator=(ReferenceCounterWithDeleter&& other) noexcept {
        if (this != &other) {
            destroy_resource();

            DeleterHolder<DeleterType>::operator=(std::move(other));
            resource_ = std::exchange(other.resource_, nullptr);
        }
        return *this;
    }

private:
    ResourceType* resource_;
};

template <typename ResourceType, typename DeleterType>
inline ReferenceCounter* new_reference_counter_with_deleter(ResourceType* resource, DeleterType&& deleter) noexcept {
    // Use std::nothrow to prevent exceptions during allocation.
    return static_cast<ReferenceCounter*>(
        new (std::nothrow) ReferenceCounterWithDeleter<ResourceType, DeleterType>(
            resource, std::move(deleter)));
}

template <typename ResourceType>
inline ReferenceCounter* new_default_reference_counter(ResourceType* resource) noexcept {
    return new_reference_counter_with_deleter<ResourceType, DefaultDeleter<ResourceType>>(
        resource, DefaultDeleter<ResourceType>());
}

}  // namespace licht