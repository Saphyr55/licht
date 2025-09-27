#include "camera.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/platform/input.hpp"

namespace licht {

void camera_update_position(Camera& camera) {
    
    if (Input::key_is_down(Key::Z)) {
        camera.position = camera.position + camera.front * camera.movement_speed;
    }
    if (Input::key_is_down(Key::S)) {
        camera.position = camera.position - camera.front * camera.movement_speed;
    }
    if (Input::key_is_down(Key::D)) {
        camera.position = camera.position + camera.right * camera.movement_speed;
    }
    if (Input::key_is_down(Key::Q)) {
        camera.position = camera.position - camera.right * camera.movement_speed;
    }

}

}  //namespace licht