#pragma once

#include "shared_ref.hpp"

namespace licht {
    
template <typename To, typename From>
inline SharedRef<To> static_ref_cast(const SharedRef<From>& other) noexcept {
    To* to = reinterpret_cast<To*>(other.get_resource());
    return SharedRef<To>(to, other.get_reference_counter());
}

template <typename T>
constexpr void* cast_to_void(const T& type) {
    return reinterpret_cast<void*>(type);
}

}  //namespace licht