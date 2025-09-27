#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/core/math/vector3.hpp"

namespace licht {

struct Quaternion {
    float32 x;
    float32 y;
    float32 z;
    float32 w;

    static float32 normal(const Quaternion& quaternion);

    static Quaternion normalize(const Quaternion& quaternion);

    static Quaternion conjugate(const Quaternion& quaternion);

    Quaternion operator*(const Quaternion& quaternion) const;
    Quaternion operator*(const Vector3f& vec) const;

    Matrix4f rotation_matrix(const Quaternion& q);

    Quaternion from_axis_angle(Vector3f axis, float32 angle);

    constexpr Quaternion() = default;
    constexpr Quaternion(float32 x, float32 y, float32 z, float32 w);
    ~Quaternion() = default;
};

}  //namespace licht