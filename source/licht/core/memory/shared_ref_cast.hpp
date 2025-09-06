#pragma once

#include <concepts>
#include <memory>

#include "shared_ref.hpp"

namespace licht {
    
template <typename To, typename From>
inline SharedRef<To> static_ref_cast(const SharedRef<From>& other) noexcept {
    To* to = static_cast<To*>(other.get_resource());
    return SharedRef<To>(to, other.get_reference_counter());
}

}  //namespace licht