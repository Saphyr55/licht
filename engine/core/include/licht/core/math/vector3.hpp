#pragma once

#include "licht/core/math/math.hpp"
#include "licht/core/math/vector2.hpp"

namespace licht {

template <Real R = float32>
struct Vector3 {
    R x = 0;
    R y = 0;
    R z = 0;

    constexpr Vector3() = default;
    constexpr Vector3(R x, R y, R z);
    constexpr Vector3(Vector2<R> vec, R z);
    constexpr explicit Vector3(R r);

    auto dot(const Vector3& vec) const;

    Vector3 operator*(const Real auto&) const;
    Vector3 operator+(const Real auto&) const;
    Vector3 operator-(const Real auto&) const;
    Vector3 operator*(const Vector3& vec) const;
    Vector3 operator+(const Vector3& vec) const;
    Vector3 operator-(const Vector3& vec) const;
    Vector3 operator-() const;

    Vector3& operator+=(const Vector3& vec);
    Vector3& operator-=(const Vector3& vec);
    Vector3& operator*=(const Vector3& vec);
    Vector3& operator+=(const Real auto& t);
    Vector3& operator-=(const Real auto& t);
    Vector3& operator*=(const Real auto& t);
    Vector3& operator/=(const Real auto& t);

    constexpr bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    constexpr bool operator!=(const Vector3& other) const {
        return !(*this == other);
    }

    static constexpr Vector3 forward();
    static constexpr float32 length(const Vector3<R>& vec);
    static constexpr Vector3 normalize(const Vector3<R>& vec);
    static constexpr Vector3 cross(const Vector3<R>& v1, const Vector3<R>& v2);
    static constexpr auto dot(const Vector3& v1, const Vector3& v2);
};

using Vector3f = Vector3<float32>;
using Vector3f32 = Vector3<float32>;
using Vector3f64 = Vector3<float64>;
using Vector3i = Vector3<int32>;
using Vector3u = Vector3<uint32>;

template <Real R>
constexpr Vector3<R>::Vector3(Vector2<R> vec, R z)
    : Vector3(vec.x, vec.y, z) {
}

template <Real R>
constexpr Vector3<R>::Vector3(R x, R y, R z)
    : x(x), y(y), z(z) {
}

template <Real R>
constexpr Vector3<R>::Vector3(R r)
    : Vector3(r, r, r) {
}

template <Real R>
constexpr Vector3<R> Vector3<R>::forward() {
    return Vector3<R>(R(0.0), R(0.0), R(1.0));
}

template <Real R>
constexpr float32 Vector3<R>::length(const Vector3& vec) {
    return Math::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

template <Real R>
constexpr Vector3<R> Vector3<R>::normalize(const Vector3& vec) {
    auto l = length(vec);
    return Vector3(vec.x / l, vec.y / l, vec.z / l);
}

template <Real R>
constexpr Vector3<R> Vector3<R>::cross(const Vector3& v1, const Vector3& v2) {
    return Vector3((v1.y * v2.z - v1.z * v2.y), 0, 0) -
           Vector3(0, (v1.x * v2.z - v1.z * v2.x), 0) -
           Vector3(0, 0, (v1.x * v2.y - v1.y * v2.x));
}

template <Real R>
constexpr auto Vector3<R>::dot(const Vector3& v1, const Vector3& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <Real R>
Vector3<R> Vector3<R>::operator-() const {
    return Vector3{-x, -y, -z};
}

template <Real R>
auto Vector3<R>::dot(const Vector3& vec) const {
    return vec.x * x + vec.y * y + vec.z * z;
}

template <Real R>
Vector3<R> Vector3<R>::operator*(const Real auto& t) const {
    return Vector3{x * t, y * t, z * t};
}

template <Real R>
Vector3<R> Vector3<R>::operator+(const Real auto& t) const {
    return Vector3{x + t, y + t, z + t};
}

template <Real R>
Vector3<R> Vector3<R>::operator-(const Real auto& t) const {
    return Vector3{x - t, y - t, z - t};
}

template <Real R>
Vector3<R> Vector3<R>::operator*(const Vector3& vec) const {
    return Vector3{vec.x * x, vec.y * y, vec.z * z};
}

template <Real R>
Vector3<R> Vector3<R>::operator+(const Vector3& vec) const {
    return Vector3{vec.x + x, vec.y + y, vec.z + z};
}

template <Real R>
Vector3<R> Vector3<R>::operator-(const Vector3& vec) const {
    return Vector3{vec.x - x, vec.y - y, vec.z - z};
}

template <Real R>
Vector3<R>& Vector3<R>::operator+=(const Vector3& vec) {
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}

template <Real R>
Vector3<R>& Vector3<R>::operator-=(const Vector3& vec) {
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return *this;
}

template <Real R>
Vector3<R>& Vector3<R>::operator*=(const Vector3& vec) {
    x *= vec.x;
    y *= vec.y;
    z *= vec.z;
    return *this;
}

template <Real R>
Vector3<R>& Vector3<R>::operator+=(const Real auto& t) {
    x += t;
    y += t;
    z += t;
    return *this;
}

template <Real R>
Vector3<R>& Vector3<R>::operator-=(const Real auto& t) {
    x -= t;
    y -= t;
    z -= t;
    return *this;
}

template <Real R>
Vector3<R>& Vector3<R>::operator*=(const Real auto& t) {
    x *= t;
    y *= t;
    z *= t;
    return *this;
}

template <Real R>
Vector3<R>& Vector3<R>::operator/=(const Real auto& t) {
    x /= t;
    y /= t;
    z /= t;
    return *this;
}

}  // namespace licht
