#pragma once

#include "licht/core/math/vector3.hpp"
namespace licht {

struct Camera {
    Vector3f pos;
    Vector3f front;
    Vector3f up;

    Vector3f target() const;

    Vector3f right() const;

    Camera(Camera&&) = default;
    Camera(const Camera&) = default;
    ~Camera() = default;
};

}  //namespace licht