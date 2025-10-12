#pragma once

#include "licht/core/math/math.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/core/math/vector3.hpp"

namespace licht {

struct Camera {
public:
    Vector3f rotation;
    Vector3f position;
    Vector3f front;
    Vector3f right = Vector3f(-1.0f, 0.0f, 0.0f);
    Vector3f up;
    Vector3f world_up = Vector3f(0.0f, 1.0f, 0.0f);

    Matrix4f view = Matrix4f(1.0f);

    float32 movement_speed = 1.0f;
    float32 mouse_sensitivity = 0.15f;

public:
    Camera(const Vector3f& position, const Vector3f& world_up = Vector3f(0.0f, 1.0f, 0.0f))
        : position(position), world_up(world_up), rotation(0.0f, 0.0f, 0.0f) {
        update_vectors();
    }

    inline void look_at(const Vector3f& target) {
        front = Vector3f::normalize(position - target);
        rotation.x = Math::degrees(Math::atan2(front.z, front.x));
        rotation.y = Math::degrees(Math::asin(front.y));
        Math::clamp(rotation.y, -89.0f, 89.0f);
        update_vectors();
    }

    inline void look_around(float32 dx, float32 dy) {
        rotation.x = Math::mod(rotation.x + dx * mouse_sensitivity, 360.0f);
        rotation.y = Math::clamp(rotation.y + dy * mouse_sensitivity, -89.0f, 89.0f);
        update_vectors();
    }

    inline void update_view() {
        view = Matrix4f::look_at(position, position + front, up);
    }

    inline void update_vectors() {
        float32 rad_yaw = Math::radians(rotation.x);
        float32 rad_pitch = Math::radians(rotation.y);

        front.x = Math::cos(rad_yaw) * Math::cos(rad_pitch);
        front.y = Math::sin(rad_pitch);
        front.z = Math::sin(rad_yaw) * Math::cos(rad_pitch);

        right = Vector3f::normalize(Vector3f::cross(front, world_up));
        up = Vector3f::normalize(Vector3f::cross(front, right));
    }
};

void camera_update(Camera& camera, float64 delta_time);

}  //namespace licht