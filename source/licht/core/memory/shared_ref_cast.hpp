#pragma once

#include <concepts>

#include "shared_ref.hpp"

namespace licht {

template <typename To, typename From>
    requires std::derived_from<From, To> || std::derived_from<To, From>
inline SharedRef<To> static_ref_cast(const SharedRef<From>& other) noexcept {
    SharedRef<To> result;
    result.resource_ = static_cast<To*>(other.resource_);
    result.reference_counter_ = other.reference_counter_;
    if (result.reference_counter_) {
        result.reference_counter_->add_shared_reference();
    }
    return result;
}

template <typename To, typename From>
inline SharedRef<To> dynamic_ref_cast(const SharedRef<From>& other) noexcept {
    SharedRef<To> result;
    if (other.resource_) {
        if (auto casted = dynamic_cast<To*>(other.resource_)) {
            result.resource_ = casted;
            result.reference_counter_ = other.reference_counter_;
            if (result.reference_counter_) {
                result.reference_counter_->add_shared_reference();
            }
        }
    }
    return result;
}

template <typename To, typename From>
inline SharedRef<To> const_ref_cast(const SharedRef<From>& other) noexcept {
    SharedRef<To> result;
    result.resource_ = const_cast<To*>(other.resource_);
    result.reference_counter_ = other.reference_counter_;
    if (result.reference_counter_) {
        result.reference_counter_->add_shared_reference();
    }
    return result;
}

template <typename To, typename From>
inline SharedRef<To> reinterpret_ref_cast(const SharedRef<From>& other) noexcept {
    SharedRef<To> result;
    result.resource_ = reinterpret_cast<To*>(other.resource_);
    result.reference_counter_ = other.reference_counter_;
    if (result.reference_counter_) {
        result.reference_counter_->add_shared_reference();
    }
    return result;
}

}  //namespace licht