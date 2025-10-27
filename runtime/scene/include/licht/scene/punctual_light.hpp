#pragma once

#include "licht/core/math/vector3.hpp"

namespace licht {

struct PunctualLight {
    alignas(16) Vector3f position;
    alignas(16) Vector3f color;
};

}