#pragma once

#include "licht/core/math/matrix4.hpp"

namespace licht {

struct UniformBufferObject {
    alignas(16) Matrix4f model;
    alignas(16) Matrix4f view;
    alignas(16) Matrix4f proj;

    UniformBufferObject() 
        : model(Matrix4f::identity())
        , view(Matrix4f::identity())
        , proj(Matrix4f::identity()) { }

    UniformBufferObject(Matrix4f in_model,
                        Matrix4f in_view,
                        Matrix4f in_proj)
        : model(in_model)
        , view(in_view)
        , proj(in_proj) { }
};

}
