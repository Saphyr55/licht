#pragma once

#include "licht/core/math/common_math.hpp"
#include "licht/core/math/vector3.hpp"

namespace licht {

template <Real R>
struct Matrix3 {
    using Mat = Matrix3<R>;
    using Vec = Vector3<R>;

    Mat operator*(Mat m);
    Vec operator*(Vec v);
    Vec& operator[](size_t i);
    Vec col(uint32 index) const;

    Matrix3() = default;
    Matrix3(R r);
    Matrix3(R m11, R m21, R m31, R m12, R m22, R m32, R m13, R m23, R m33);
    Matrix3(const Vec& col_1, const Vec& col_2, const Vec& col_3);

    static Matrix3 identity();
    //static std::array<R, 3 * 3> ValueArray(Mat mat);
    static Matrix3 translate(Mat mat, Vector3<R> vec);
    static Matrix3 scale(Mat mat, Vector3<R> vec);
    static Matrix3 rotate(Mat mat, const Real auto& theta, Vector3<R> vec);
    static Matrix3 rotate_x(float32 theta);
    static Matrix3 rotate_y(float32 theta);
    static Matrix3 rotate_z(float32 theta);

private:
    Vec value[3];
};

using Matrix3f = Matrix3<float32>;
using Matrix3f32 = Matrix3<float32>;
using Matrix3f64 = Matrix3<float64>;

template <Real R>
Matrix3<R> Matrix3<R>::identity() {
    return Matrix3(1);
}

template <Real R>
Matrix3<R> Matrix3<R>::translate(Mat mat, Vector3<R> vec) {
    Mat transformation_mat(1);
    transformation_mat[2] = vec;
    return mat * transformation_mat;
}

template <Real R>
Matrix3<R> Matrix3<R>::scale(Mat mat, Vector3<R> vec) {
    return mat * Mat(
                     Vector3<R>(vec.x, 0, 0),
                     Vector3<R>(0, vec.y, 0),
                     Vector4<R>(0, 0, vec.z));
}

template <Real R>
Matrix3<R> Matrix3<R>::rotate(Mat mat, const Real auto& theta, Vector3<R> vec) {
    auto c = rcos(theta);
    auto s = rsin(theta);
    auto v = Vector3<R>::Normalize(vec);

    return mat * Mat(
                     c + v.x * v.x * (1 - c), v.x * v.y * (1 - c) - v.z * s, v.x * v.z * (1 - c) + v.y * s,
                     v.x * v.y * (1 - c) + v.z * s, c + v.y * v.y * (1 - c), v.y * v.z * (1 - c) - v.x * s,
                     v.x * v.z * (1 - c) - v.y * s, v.z * v.y * (1 - c) + v.x * s, c + v.z * v.z * (1 - c));
}

template <Real R>
Matrix3<R> Matrix3<R>::rotate_x(const float32 theta) {
    return Mat(
        Vector3<R>(1, 0, 0),
        Vector3<R>(0, rcos(theta), rsin(theta)),
        Vector3<R>(0, -rsin(theta), rcos(theta)));
}

template <Real R>
Matrix3<R> Matrix3<R>::rotate_y(const float32 theta) {
    return Mat(
        Vector3<R>(rcos(theta), 0, -rsin(theta)),
        Vector3<R>(0, 1, 0),
        Vector3<R>(rsin(theta), 0, rcos(theta)));
}

template <Real R>
Matrix3<R> Matrix3<R>::rotate_z(const float32 theta) {
    return Matrix3<R>(
        Vector3<R>(rcos(theta), rsin(theta), 0),
        Vector3<R>(-rsin(theta), rcos(theta), 0),
        Vector3<R>(0, 0, 1));
}

template <Real R>
Matrix3<R>::Vec Matrix3<R>::col(uint32 index) const {
    return (*this)[index];
}

template <Real R>
Matrix3<R>::Matrix3(const Vec& col_1, const Vec& col_2, const Vec& col_3) {
    value[0] = col_1;
    value[1] = col_2;
    value[2] = col_3;
}

template <Real R>
Matrix3<R>::Matrix3(R m11, R m21, R m31, R m12, R m22, R m32, R m13, R m23, R m33)
    : Matrix3(
          Vec(m11, m12, m13),
          Vec(m21, m22, m23),
          Vec(m31, m32, m33)) {
}

template <Real R>
Matrix3<R>::Matrix3(R r)
    : Matrix3(
          Vec(r, 0, 0),
          Vec(0, r, 0),
          Vec(0, 0, r)) {
}

template <Real R>
auto Matrix3<R>::operator*(Mat m) -> Mat {
    Mat result(0);
    for (int8 i = 0; i < 3; i++) {
        for (int8 j = 0; j < 3; j++) {
            R r_ji{};
            for (int8 k = 0; k < 3; k++) {
                r_ji += m[j][k] * value[k][i];
            }
            result[j][i] = r_ji;
        }
    }
    return result;
}

template <Real R>
Vector3<R> Matrix3<R>::operator*(Vector3<R> v) {
    return Vec(
        (*this)[0][0] * v[0] + (*this)[0][1] * v[1] + (*this)[0][2] * v[2],
        (*this)[1][0] * v[0] + (*this)[1][1] * v[1] + (*this)[1][2] * v[2],
        (*this)[2][0] * v[0] + (*this)[2][1] * v[1] + (*this)[2][2] * v[2]);
}

template <Real R>
Vector3<R>& Matrix3<R>::operator[](size_t i) {
    return value[i];
}

}  //namespace licht
