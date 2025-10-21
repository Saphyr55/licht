#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/rhi/buffer.hpp"

namespace licht {

struct RenderModelConstant {
    alignas(64) Matrix4f model = Matrix4f::identity();

    RenderModelConstant() = default;
};

struct RenderPunctualLight {
    alignas(16) Vector3f position;
    alignas(16) Vector3f color;
};

struct TextureFactors {
    alignas(16) Vector4f diffuse_factor;
};

struct DrawItem {
    Array<RHIBuffer*> vertex_buffers;
    RHIBuffer* index_buffer;
    size_t index_start = 0;
    size_t index_count = 0;
    size_t vertex_start = 0;
    size_t vertex_count = 0;

    Array<RHISampler*> samplers;
    Array<RHITexture*> textures;
    Array<RHITextureView*> texture_views;
    Array<Array<TextureFactors>> texture_factors_list;

    Array<RHIShaderResourceGroup*> shader_groups;

    // TODO: Remove this
    RenderModelConstant model_constant;
};

struct RenderPacket {
    Array<DrawItem> items;
    RenderPunctualLight light;
};

}  //namespace licht