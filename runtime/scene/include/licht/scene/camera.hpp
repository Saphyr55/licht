#pragma once

#include "licht/core/math/matrix4.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/scene/scene_exports.hpp"

namespace licht {

struct LICHT_SCENE_API Camera {
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
    void look_at(const Vector3f& target);

    void look_around(float32 dx, float32 dy);

    void update_view();

    void update_vectors();

public:
    Camera(const Vector3f& position, const Vector3f& world_up = Vector3f(0.0f, 1.0f, 0.0f))
        : position(position), world_up(world_up), rotation(0.0f, 0.0f, 0.0f) {
        update_vectors();
    }
};

}  //namespace licht