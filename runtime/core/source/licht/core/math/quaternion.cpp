#include "licht/core/math/quaternion.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/math/math.hpp"

namespace licht {

constexpr Quaternion Quaternion::from_axis_angle(const Vector3f& axis, float32 angle) {
    const float32 half_angle = 0.5f * angle;
    float32 half_angle_sin = Math::sin(half_angle);
    float32 half_angle_cos = Math::cos(half_angle);

    return {
        half_angle_sin * axis.x,
        half_angle_sin * axis.y,
        half_angle_sin * axis.z,
        half_angle_cos};
}

constexpr Matrix4f Quaternion::rotation_matrix(const Quaternion& q) {
    Matrix4f result = Matrix4f::identity();
    float32 qxx(q.x * q.x);
    float32 qyy(q.y * q.y);
    float32 qzz(q.z * q.z);
    float32 qxz(q.x * q.z);
    float32 qxy(q.x * q.y);
    float32 qyz(q.y * q.z);
    float32 qwx(q.w * q.x);
    float32 qwy(q.w * q.y);
    float32 qwz(q.w * q.z);

    result[0][0] = float32(1) - float32(2) * (qyy + qzz);
    result[0][1] = float32(2) * (qxy + qwz);
    result[0][2] = float32(2) * (qxz - qwy);

    result[1][0] = float32(2) * (qxy - qwz);
    result[1][1] = float32(1) - float32(2) * (qxx + qzz);
    result[1][2] = float32(2) * (qyz + qwx);

    result[2][0] = float32(2) * (qxz + qwy);
    result[2][1] = float32(2) * (qyz - qwx);
    result[2][2] = float32(1) - float32(2) * (qxx + qyy);

    return result;
}

constexpr Quaternion::Quaternion(float32 x,
                                 float32 y,
                                 float32 z,
                                 float32 w)
    : x(x)
    , y(y)
    , z(z)
    , w(w) {
}

constexpr float32 Quaternion::normal(const Quaternion& q) {
    return Math::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

constexpr Quaternion Quaternion::normalize(const Quaternion& quaternion) {
    float32 quaternionNorm = normal(quaternion);
    return {
        quaternion.x / quaternionNorm,
        quaternion.y / quaternionNorm,
        quaternion.z / quaternionNorm,
        quaternion.w / quaternionNorm};
}

constexpr Quaternion Quaternion::conjugate(const Quaternion& quaternion) {
    return {
        -quaternion.x,
        -quaternion.y,
        -quaternion.z,
        quaternion.w};
}

constexpr Quaternion Quaternion::operator*(const Quaternion& q) const {
    const float32 new_x = x * q.w + w * q.x + y * q.z - z * q.y;
    const float32 new_y = y * q.w + w * q.y + z * q.x - x * q.z;
    const float32 new_z = z * q.w + w * q.z + x * q.y - y * q.x;
    const float32 new_w = w * q.w - x * q.x - y * q.y - z * q.z;

    return {new_x, new_y, new_z, new_w};
}

constexpr Quaternion Quaternion::operator*(const Vector3f& vec) const {
    const float32 new_x = w * vec.x + y * vec.z - z * vec.y;
    const float32 new_y = w * vec.y + z * vec.x - x * vec.z;
    const float32 new_z = w * vec.z + x * vec.y - y * vec.x;
    const float32 new_w = -x * vec.x - y * vec.y - z * vec.z;

    return {new_x, new_y, new_z, new_w};
}

}  //namespace licht