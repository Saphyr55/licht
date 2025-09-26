#include "camera.hpp"

namespace licht {

Vector3f Camera::target() const {
    return pos + front;
};

Vector3f Camera::right() const {
    return Vector3f::normalize(Vector3f::cross(target(), up));
}

}  //namespace licht