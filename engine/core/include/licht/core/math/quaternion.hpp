#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/core_exports.hpp"

namespace licht {

struct LICHT_CORE_API Quaternion {
    float32 x = 0.0f;
    float32 y = 0.0f;
    float32 z = 0.0f;
    float32 w = 1.0f;

    static constexpr float32 normal(const Quaternion& quaternion);

    static constexpr Quaternion normalize(const Quaternion& quaternion);

    static constexpr Quaternion conjugate(const Quaternion& quaternion);

    static constexpr Matrix4f rotation_matrix(const Quaternion& q);

    static constexpr Quaternion from_axis_angle(const Vector3f& axis, float32 angle);

    constexpr Quaternion operator*(const Quaternion& quaternion) const;
    constexpr Quaternion operator*(const Vector3f& vec) const;

    constexpr Quaternion() = default;
    constexpr Quaternion(float32 x, float32 y, float32 z, float32 w);
    ~Quaternion() = default;
};

}  //namespace licht