#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/rhi/buffer.hpp"

namespace licht {

struct RenderModelConstant {
    alignas(16) Matrix4f model = Matrix4f::identity();

    RenderModelConstant() = default;
};

struct RenderItem {
    Array<RHIBuffer*> vertex_buffers;

    RHIBuffer* index_buffer;
    size_t index_count = 0;

    RHISampler* sampler;

    RHITexture* texture;
    RHITextureView* texture_view;

    Array<RHIShaderResourceGroup*> shader_groups;

    RenderModelConstant model_constant;
};

struct RenderPacket {
    Array<RenderItem> items;
};

}  //namespace licht