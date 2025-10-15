#pragma once

#include "licht/core/defines.hpp"

#include <cfloat>
#include <cmath>

namespace licht {

template <typename RealType>
concept Real = std::is_floating_point_v<RealType> || std::is_integral_v<RealType>;

class Math {
public:

    static constexpr inline auto pi() -> auto {
        return 3.14159265358979323846264338327950288;
    }

    static constexpr inline auto polar_cap() -> auto {
        return pi() - FLT_EPSILON;
    }

    static constexpr inline auto sqrt(const Real auto& real) -> auto {
        return std::sqrt(real);
    }

    static constexpr inline auto cos(const Real auto& real) -> auto {
        return std::cos(real);
    }

    static constexpr inline auto sin(const Real auto& real) -> auto {
        return std::sin(real);
    }

    static constexpr inline auto tan(const Real auto& real) -> auto {
        return std::tan(real);
    }

    static constexpr inline auto abs(const Real auto& real) -> auto {
        return std::abs(real);
    }

    static constexpr inline auto radians(const Real auto& angle) -> auto {
        return angle * (pi() / 180.0);
    }

    static constexpr inline auto degrees(const Real auto& angle) -> auto {
        return angle * (180.0 / pi());
    }

    static constexpr inline auto signum(const Real auto& r) -> auto {
        return r == decltype(r)(0) ? 0 : rabs(r) / r;
    }

    static constexpr inline auto clamp(const auto& value, const auto& min, const auto& max) -> auto {
        return (value < min) ? min : (value > max) ? max
                                                   : value;
    }

    static constexpr inline auto atan(const Real auto& value) -> auto {
        return std::atan(value);
    }

    static constexpr inline auto atan2(const Real auto& value1, const Real auto& value2) -> auto {
        return std::atan2(value1, value2);
    }

    static constexpr inline auto mod(const Real auto& value, const Real auto& degree) -> auto {
        return std::fmod(value, degree);
    }

    static constexpr inline auto asin(const Real auto& value) -> auto {
        return std::asin(value);
    }

    static constexpr inline auto floor(const Real auto& value) -> auto {
        return std::floor(value);
    }

    static constexpr inline auto ceil(const Real auto& value) -> auto {
        return std::ceil(value);
    }

    static constexpr inline auto log(const Real auto& value) -> auto {
        return std::log(value);
    }

    static constexpr inline auto log2(const Real auto& value) -> auto {
        return std::log2(value);
    }
    static constexpr inline auto min(const auto& value, const auto& min) -> auto {
        return (value < min) ? min : value;
    }

    static constexpr inline auto max(const auto& value, const auto& max) -> auto {
        return (value > max) ? max : value;
    }
};

}  //namespace licht
