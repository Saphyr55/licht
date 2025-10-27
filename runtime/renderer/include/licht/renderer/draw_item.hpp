#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/renderer/mesh/static_mesh.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/device_memory_uploader.hpp"

namespace licht {

struct RenderModelConstant {
    alignas(16) Matrix4f model = Matrix4f::identity();
};

struct LICHT_RENDERER_API DrawItem {
public:
    static DrawItem create(SharedRef<RHIDevice> device, RHIDeviceMemoryUploader& uploader, StaticSubMesh& submesh);

    DrawItem() = default;
    ~DrawItem() = default;

public:
    Array<RHIBuffer*> vertex_buffers;
    RHIBuffer* index_buffer = nullptr;
    size_t index_start = 0;
    size_t index_count = 0;
    size_t vertex_start = 0;
    size_t vertex_count = 0;

    Array<RHISampler*> samplers;
    Array<RHITexture*> textures;
    Array<RHITextureView*> texture_views;

    Array<RHIShaderResourceGroup*> shader_groups;

    // TODO: Remove this
    RenderModelConstant model_constant;
};

struct RenderPacket {
    Array<DrawItem> items;
};

}  //namespace licht