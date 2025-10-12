#include "camera.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/platform/input.hpp"

namespace licht {

void camera_update(Camera& camera, float64 delta_time) {
    
    Vector3f direction(0.0f, 0.0f, 0.0f);

    if (Input::key_is_down(VirtualKey::Z)) {
        direction += camera.front;
    }

    if (Input::key_is_down(VirtualKey::S)) {
        direction -= camera.front;
    }

    if (Input::key_is_down(VirtualKey::Q)) {
        direction -= camera.right;
    }

    if (Input::key_is_down(VirtualKey::D)) {
        direction += camera.right;
    }

    if (Input::key_is_down(VirtualKey::Space)) {
        direction -= camera.world_up;
    }

    if (Input::key_is_down(VirtualKey::LeftShift)) {
        direction += camera.world_up;
    }

    if (Vector3f::length(direction) > 0.0f) {
        direction = Vector3f::normalize(direction);
        camera.position += direction * camera.movement_speed * delta_time;
        camera.update_vectors();
    }

    camera.update_view();
}

}  //namespace licht