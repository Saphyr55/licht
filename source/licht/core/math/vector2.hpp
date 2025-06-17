#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/math/common_math.hpp"

namespace licht {

template <Real R>
struct Vector2 {
    R x = 0;
    R y = 0;

    auto dot(const Vector2& vec) const -> auto;

    Vector2 operator*(const Real auto&) const;
    Vector2 operator+(const Real auto&) const;
    Vector2 operator-(const Real auto&) const;
    Vector2 operator*(const Vector2&) const;
    Vector2 operator+(const Vector2&) const;
    Vector2 operator-(const Vector2&) const;
    Vector2 operator-() const;

    bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }
};

using Vector2f = Vector2<float32>;
using Vector2d = Vector2<float64>;
using Vector2i = Vector2<int32>;
using Vector2u = Vector2<uint32>;

template <Real R>
auto Vector2<R>::dot(const Vector2& vec) const {
    return vec.x * x + vec.y * y;
}

template <Real R>
Vector2<R> Vector2<R>::operator*(const Real auto& t) const {
    return Vector2{
        x * t,
        y * t};
}

template <Real R>
Vector2<R> Vector2<R>::operator+(const Real auto& t) const {
    return Vector2{
        x + t,
        y + t
    };
}

template <Real R>
Vector2<R> Vector2<R>::operator-() const {
    return Vector2{
        -x,
        -y
    };
}

template <Real R>
Vector2<R> Vector2<R>::operator-(const Real auto& t) const {
    return Vector2{
        x - t,
        y - t
    };
}

template <Real R>
Vector2<R> Vector2<R>::operator*(const Vector2& vec) const {
    return Vector2{
        vec.x * x,
        vec.y * y
    };
}

template <Real R>
Vector2<R> Vector2<R>::operator+(const Vector2& vec) const {
    return Vector2{
        vec.x + x,
        vec.y + y
    };
}

template <Real R>
Vector2<R> Vector2<R>::operator-(const Vector2& vec) const {
    return Vector2{
        vec.x - x,
        vec.y - y
    };
}

}  //namespace licht
