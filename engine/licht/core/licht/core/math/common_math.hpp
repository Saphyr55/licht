#pragma once

#include <cfloat>
#include <cmath>

#include "licht/core/defines.hpp"

namespace licht {

template <typename RealType>
concept Real = std::is_floating_point_v<RealType> || std::is_integral_v<RealType>;

constexpr const float32 PI(3.14159265358979323846264338327950288);
constexpr const float32 polar_cap = PI - FLT_EPSILON;

constexpr inline auto rsqrt(const Real auto& real) {
    return std::sqrt(real);
}

constexpr inline auto rcos(const Real auto& real) {
    return std::cos(real);
}

constexpr inline auto rsin(const Real auto& real) {
    return std::sin(real);
}

constexpr inline auto rtan(const Real auto& real) {
    return std::tan(real);
}

constexpr inline auto rabs(const Real auto& real) {
    return std::abs(real);
}

constexpr inline auto radians(const Real auto& angle) {
    return angle * (PI / 180);
}

constexpr inline auto signum(const Real auto& r) {
    return r == decltype(r)(0) ? 0 : rabs(r) / r;
}

constexpr inline auto clamp(const Real auto& value, const Real auto& min, const Real auto& max) {
    return (value < min) ? min : (value > max) ? max : value;
}

}  //namespace licht
