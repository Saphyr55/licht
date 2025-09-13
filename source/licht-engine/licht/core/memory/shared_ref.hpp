#pragma once

#include <concepts>
#include <memory>

#include "licht/core/memory/reference_counter.hpp"

namespace licht {

template <typename ResourceType>
class SharedRef {
public:
    ResourceType& operator*() {
        LCHECK(resource_)
        return *resource_;
    }

    ResourceType* operator->() { return resource_; }

    const ResourceType& operator*() const {
        LCHECK(resource_)
        return *resource_;
    }

    const ResourceType* operator->() const { return resource_; }

    ResourceType* get_resource() const { return resource_; }

    ReferenceCounter* get_reference_counter() {
        return reference_counter_;
    }

    ReferenceCounter* get_reference_counter() const {
        return reference_counter_;
    }

    int32 get_shared_reference_count() const {
        return reference_counter_->get_shared_reference_count();
    }

    bool is_unique() const {
        return reference_counter_->is_unique();
    }

    template <typename DeleterType = std::default_delete<ResourceType>>
    void reset(ResourceType* resource = nullptr,
               DeleterType deleter = DeleterType()) {
        release_shared_reference();

        resource_ = resource;
        if (resource_) {
            reference_counter_ = new_reference_counter_with_deleter<ResourceType, DeleterType>(resource_, deleter);
        }
    }

    bool is_valid() const { return resource_ != nullptr; }

    explicit operator bool() const { return is_valid(); }

public:
    SharedRef() noexcept = default;

    explicit SharedRef(ResourceType* resource) noexcept
        : resource_(resource)
        , reference_counter_(nullptr) {
        if (resource_) {
            reference_counter_ = new_default_reference_counter<ResourceType>(resource_);
        }
    }
    
    SharedRef(ResourceType* resource, ReferenceCounter* reference_counter) noexcept
        : resource_(resource)
        , reference_counter_(reference_counter) {
        if (reference_counter_) {
            reference_counter_->add_shared_reference();
        }
    }

    template <typename DerivedType>
        requires std::derived_from<DerivedType, ResourceType>
    explicit SharedRef(DerivedType* resource) noexcept
        : resource_(resource) {
        if (resource_) {
            reference_counter_ = new_default_reference_counter<DerivedType>(resource_);
        }
    }

    template <typename DeleterType>
    SharedRef(ResourceType* resource, DeleterType deleter) noexcept
        : resource_(resource) {
        if (resource_) {
            reference_counter_ = new_reference_counter_with_deleter<ResourceType, DeleterType>(resource_, deleter);
        }
    }

    template <typename DerivedType, typename DeleterType>
        requires std::derived_from<DerivedType, ResourceType>
    SharedRef(DerivedType* resource, DeleterType deleter) noexcept
        : resource_(resource) {
        if (resource_) {
            reference_counter_ = new_reference_counter_with_deleter<DerivedType, DeleterType>(resource_, deleter);
        }
    }

    SharedRef(const SharedRef& other)
        : resource_(other.resource_)
        , reference_counter_(other.reference_counter_) {
        if (reference_counter_) {
            reference_counter_->add_shared_reference();
        }
    }

    template <typename DerivedType>
        requires std::derived_from<DerivedType, ResourceType>
    SharedRef(const SharedRef<DerivedType>& other) noexcept
        : resource_(other.resource_)
        , reference_counter_(other.reference_counter_) {
        if (reference_counter_) {
            reference_counter_->add_shared_reference();
        }
    }

    SharedRef& operator=(const SharedRef& other) noexcept {
        release_shared_reference();

        resource_ = static_cast<ResourceType*>(other.resource_);
        reference_counter_ = other.reference_counter_;

        if (reference_counter_) {
            reference_counter_->add_shared_reference();
        }

        return *this;
    }

    template <typename DerivedType>
        requires std::derived_from<DerivedType, ResourceType>
    SharedRef& operator=(const SharedRef<DerivedType>& other) noexcept {
        release_shared_reference();

        resource_ = static_cast<ResourceType*>(other.resource_);
        reference_counter_ = other.reference_counter_;

        if (reference_counter_) {
            reference_counter_->add_shared_reference();
        }

        return *this;
    }

    SharedRef(SharedRef&& other) noexcept
        : resource_(other.resource_)
        , reference_counter_(other.reference_counter_) {
        other.resource_ = nullptr;
        other.reference_counter_ = nullptr;
    }

    template <typename DerivedType>
        requires std::derived_from<DerivedType, ResourceType>
    SharedRef(SharedRef<DerivedType>&& other) noexcept
        : resource_(other.resource_)
        , reference_counter_(other.reference_counter_) {
        other.resource_ = nullptr;
        other.reference_counter_ = nullptr;
    }

    SharedRef& operator=(SharedRef&& other) noexcept {
        release_shared_reference();

        resource_ = other.resource_;
        reference_counter_ = other.reference_counter_;
        other.resource_ = nullptr;
        other.reference_counter_ = nullptr;

        return *this;
    }

    template <typename DerivedType>
        requires std::derived_from<DerivedType, ResourceType>
    SharedRef& operator=(SharedRef<DerivedType>&& other) noexcept {
        release_shared_reference();

        resource_ = other.resource_;
        reference_counter_ = other.reference_counter_;
        other.resource_ = nullptr;
        other.reference_counter_ = nullptr;
        return *this;
    }

    SharedRef& operator=(ResourceType* new_ptr) noexcept {
        reset(new_ptr);
        return *this;
    }

    template <typename DerivedType>
        requires std::derived_from<DerivedType, ResourceType>
    SharedRef& operator=(DerivedType* new_ptr) noexcept {
        reset(new_ptr);
        return *this;
    }

    ~SharedRef() noexcept {
        release_shared_reference();
    }

public:
    void release_shared_reference() {
        if (is_valid()) {
            reference_counter_->release_shared_reference();
        }
    }

    template <typename OtherResourceType>
    friend class SharedRef;

private:
    ResourceType* resource_ = nullptr;
    ReferenceCounter* reference_counter_ = nullptr;
};

template <typename ResourceType, typename... Args>
constexpr inline SharedRef<ResourceType> new_ref(Args&&... args) noexcept {
    return SharedRef<ResourceType>(new ResourceType(std::forward<Args>(args)...));
}

}  // namespace licht

template <typename ResourceType>
struct std::hash<licht::SharedRef<ResourceType>> {
    usize operator()(const licht::SharedRef<ResourceType>& ref) const {
        return std::hash<ResourceType*>()(ref.get_resource());
    }
};