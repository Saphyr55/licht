#include "licht/scene/camera.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/platform/input.hpp"

namespace licht {

void Camera::look_at(const Vector3f& target) {
    front = Vector3f::normalize(position - target);
    rotation.x = Math::degrees(Math::atan2(front.z, front.x));
    rotation.y = Math::degrees(Math::asin(front.y));
    Math::clamp(rotation.y, -89.0f, 89.0f);
    update_vectors();
}

void Camera::look_around(float32 dx, float32 dy) {
    rotation.x = Math::mod(rotation.x + dx * mouse_sensitivity, 360.0f);
    rotation.y = Math::clamp(rotation.y + dy * mouse_sensitivity, -89.0f, 89.0f);
    update_vectors();
}

void Camera::update_view() {
    view = Matrix4f::look_at(position, position + front, up);
}

void Camera::update_vectors() {
    float32 rad_yaw = Math::radians(rotation.x);
    float32 rad_pitch = Math::radians(rotation.y);

    front.x = Math::cos(rad_yaw) * Math::cos(rad_pitch);
    front.y = Math::sin(rad_pitch);
    front.z = Math::sin(rad_yaw) * Math::cos(rad_pitch);

    right = Vector3f::normalize(Vector3f::cross(front, world_up));
    up = Vector3f::normalize(Vector3f::cross(front, right));
}

}  //namespace licht