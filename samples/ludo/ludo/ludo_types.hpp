#pragma once

#include "licht/core/math/matrix4.hpp"

namespace licht {

struct UniformBufferObject {
    Matrix4f model;
    Matrix4f view;
    Matrix4f proj;
};

}
