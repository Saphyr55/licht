#pragma once

#include "licht/core/math/matrix4.hpp"

namespace licht {

struct UniformBufferObject {
    alignas(16) Matrix4f model = Matrix4f::identity();
    alignas(16) Matrix4f view = Matrix4f::identity();
    alignas(16) Matrix4f proj = Matrix4f::identity();

    UniformBufferObject() = default;
};

}
