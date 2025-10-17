#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/rhi/buffer.hpp"

namespace licht {

struct RenderModelConstant {
    alignas(16) Matrix4f model = Matrix4f::identity();

    RenderModelConstant() = default;
};

struct RenderPunctualLight {
    alignas(16) Vector3f position;
    alignas(16) Vector3f color;
};

struct RenderItem {
    Array<RHIBuffer*> vertex_buffers;

    RHIBuffer* index_buffer;
    size_t index_count = 0;

    Array<RHISampler*> samplers;
    Array<RHITexture*> textures;
    Array<RHITextureView*> texture_views;


    Array<RHIShaderResourceGroup*> shader_groups;

    // TODO: Remove this
    RenderModelConstant model_constant;
};

struct RenderPacket {
    Array<RenderItem> items;
    RenderPunctualLight light;
};

}  //namespace licht