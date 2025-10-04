#include "camera.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/platform/input.hpp"

namespace licht {

void camera_update_position(Camera& camera) {

    if (Input::key_is_down(VirtualKey::Z)) {
        camera.position += camera.front * camera.movement_speed;
        camera.update_view();
    }

    if (Input::key_is_down(VirtualKey::S)) {
        camera.position -= camera.front * camera.movement_speed;
        camera.update_view();
    }
    
    if (Input::key_is_down(VirtualKey::Q)) {
        camera.position -= camera.right * camera.movement_speed;
        camera.update_view();
    }
    
    if (Input::key_is_down(VirtualKey::D)) {
        camera.position += camera.right * camera.movement_speed;
        camera.update_view();
    }

    if (Input::key_is_down(VirtualKey::Space)) {
        camera.position -= camera.world_up * camera.movement_speed;
        camera.update_view();
    }

    if (Input::key_is_down(VirtualKey::LeftShift)) {
        camera.position += camera.world_up * camera.movement_speed;
        camera.update_view();
    }

}

}  //namespace licht