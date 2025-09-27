#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/math/math.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/core/math/vector3.hpp"

namespace licht {

struct Camera {
public:
    Vector3f position;
    Vector3f front;
    Vector3f up;
    Vector3f right;
    Vector3f world_up;

    float32 yaw;
    float32 picth;

    float32 movement_speed = 0.0015f;
    float32 mouse_sensitivity = 0.5f;

public:
    Camera(Vector3f position, Vector3f front, Vector3f up = Vector3f(0.0f, 0.0f, 1.0f), float32 yaw = -90.0f, float32 pitch = 0.0f)
        : position(position), front(front), world_up(up), up(up), yaw(yaw), picth(pitch) {
        // update();
    }

    inline Matrix4f view() const {
        return Matrix4f::look_at(position, position + front, up);
    }

    inline void look_around(float32 dx, float32 dy) {
        yaw += dx * mouse_sensitivity;
        picth -= dy * mouse_sensitivity;
        picth = Math::clamp(picth, -89.0f, 89.0f);

        // update();
    }

private:
    void update() {
        Vector3f front;
        front.x = Math::cos(Math::radians(yaw)) * Math::cos(Math::radians(picth));
        front.y = Math::sin(Math::radians(picth));
        front.z = Math::sin(Math::radians(yaw)) * Math::cos(Math::radians(picth));
        front = Vector3f::normalize(front);

        right = Vector3f::normalize(Vector3f::cross(front, world_up));
        up = Vector3f::normalize(Vector3f::cross(right, front));
    }

};

void camera_update_position(Camera& camera);

}  //namespace licht