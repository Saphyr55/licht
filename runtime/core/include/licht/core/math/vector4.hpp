#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/math/vector3.hpp"

namespace licht {

template <Real R = float32>
struct Vector4 {
    R x = 0;
    R y = 0;
    R z = 0;
    R w = 0;

    constexpr Vector4() = default;
    constexpr explicit Vector4(R r);
    constexpr explicit Vector4(Vector3<R> vec, R w = 1);
    constexpr Vector4(R x, R y, R z, R w);

    constexpr auto dot(const Vector4& vec) const;
    constexpr R& operator[](size_t i);
    constexpr const R& operator[](size_t i) const;

    constexpr Vector4 operator*(const Real auto&) const;
    constexpr Vector4 operator+(const Real auto&) const;
    constexpr Vector4 operator-(const Real auto&) const;
    constexpr Vector4 operator/(const Real auto&) const;
    constexpr Vector4 operator*(const Vector4& vec) const;
    constexpr Vector4 operator/(const Vector4& vec) const;
    constexpr Vector4 operator+(const Vector4& vec) const;
    constexpr Vector4 operator-(const Vector4& vec) const;
    constexpr Vector4 operator-() const;

    Vector4& operator+=(const Vector4& vec);
    Vector4& operator-=(const Vector4& vec);
    Vector4& operator*=(const Vector4& vec);
    Vector4& operator/=(const Vector4& vec);
    Vector4& operator+=(const Real auto& t);
    Vector4& operator-=(const Real auto& t);
    Vector4& operator*=(const Real auto& t);
    Vector4& operator/=(const Real auto& t);

    static constexpr float32 norm(Vector4 vec);
    static constexpr Vector4<float32> normalize(Vector4 vec);
};

using Vector4f = Vector4<float32>;
using Vector4f32 = Vector4<float32>;
using Vector4f64 = Vector4<float64>;
using Vector4i = Vector4<int32>;
using Vector4u = Vector4<uint32>;

template <Real R>
constexpr Vector4<R>::Vector4(Vector3<R> vec, R w)
    : Vector4(vec.x, vec.y, vec.z, w) {
}

template <Real R>
constexpr Vector4<R>::Vector4(R x, R y, R z, R w)
    : x(x), y(y), z(z), w(w) {
}

template <Real R>
constexpr Vector4<R>::Vector4(R r)
    : Vector4(r, r, r, r) {
}

template <Real R>
constexpr auto Vector4<R>::dot(const Vector4& vec) const {
    return vec.x * x + vec.y * y + vec.z * z + vec.w * w;
}

template <Real R>
constexpr float32 Vector4<R>::norm(Vector4 vec) {
    return Math::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}

template <Real R>
constexpr Vector4f Vector4<R>::normalize(Vector4 vec) {
    float32 l = norm(vec);
    return Vector4f{
        vec.x / l,
        vec.y / l,
        vec.z / l,
        vec.w / l};
}

template <Real R>
constexpr R& Vector4<R>::operator[](const size_t i) {
    LCHECK(i < 4)
    switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        default:
            LCRASH("");
    }
    return x;
}

template <Real R>
constexpr const R& Vector4<R>::operator[](size_t i) const {
    LCHECK(i < 4)
    switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        default:
            LCRASH("");
    }
    return x;
}

template <Real R>
constexpr Vector4<R> Vector4<R>::operator*(const Real auto& t) const {
    return Vector4{x * t, y * t, z * t, w * t};
}

template <Real R>
constexpr Vector4<R> Vector4<R>::operator+(const Real auto& t) const {
    return Vector4{x + t, y + t, z + t, w + t};
}

template <Real R>
constexpr Vector4<R> Vector4<R>::operator-(const Real auto& t) const {
    return Vector4{x - t, y - t, z - t, w - t};
}

template <Real R>
constexpr Vector4<R> Vector4<R>::operator/(const Real auto& t) const {
    return Vector4{x / t, y / t, z / t, w / t};
}

template <Real R>
constexpr Vector4<R> Vector4<R>::operator-() const {
    return Vector4{-x, -y, -z, -w};
}

template <Real R>
constexpr Vector4<R> Vector4<R>::operator*(const Vector4& vec) const {
    return Vector4{x * vec.x, y * vec.y, z * vec.z, w * vec.w};
}

template <Real R>
constexpr Vector4<R> Vector4<R>::operator/(const Vector4& vec) const {
    return Vector4{x / vec.x, y / vec.y, z / vec.z, w / vec.w};
}

template <Real R>
constexpr Vector4<R> Vector4<R>::operator+(const Vector4& vec) const {
    return Vector4{x + vec.x, y + vec.y, z + vec.z, w + vec.w};
}

template <Real R>
constexpr Vector4<R> Vector4<R>::operator-(const Vector4& vec) const {
    return Vector4{x - vec.x, y - vec.y, z - vec.z, w - vec.w};
}

template <Real R>
Vector4<R>& Vector4<R>::operator+=(const Vector4& vec) {
    x += vec.x;
    y += vec.y;
    z += vec.z;
    w += vec.w;
    return *this;
}

template <Real R>
Vector4<R>& Vector4<R>::operator-=(const Vector4& vec) {
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    w -= vec.w;
    return *this;
}

template <Real R>
Vector4<R>& Vector4<R>::operator*=(const Vector4& vec) {
    x *= vec.x;
    y *= vec.y;
    z *= vec.z;
    w *= vec.w;
    return *this;
}

template <Real R>
Vector4<R>& Vector4<R>::operator/=(const Vector4& vec) {
    x /= vec.x;
    y /= vec.y;
    z /= vec.z;
    w /= vec.w;
    return *this;
}

template <Real R>
Vector4<R>& Vector4<R>::operator+=(const Real auto& t) {
    x += t;
    y += t;
    z += t;
    w += t;
    return *this;
}

template <Real R>
Vector4<R>& Vector4<R>::operator-=(const Real auto& t) {
    x -= t;
    y -= t;
    z -= t;
    w -= t;
    return *this;
}

template <Real R>
Vector4<R>& Vector4<R>::operator*=(const Real auto& t) {
    x *= t;
    y *= t;
    z *= t;
    w *= t;
    return *this;
}

template <Real R>
Vector4<R>& Vector4<R>::operator/=(const Real auto& t) {
    x /= t;
    y /= t;
    z /= t;
    w /= t;
    return *this;
}

}  // namespace licht
