#include "licht/renderer/draw_item.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/device_memory_uploader.hpp"
#include "licht/rhi/texture.hpp"

namespace licht {

DrawItem DrawItem::create(SharedRef<RHIDevice> device, RHIDeviceMemoryUploader& uploader, StaticSubMesh& submesh) {
    DrawItem item;

    item.model_constant.model = Matrix4f::identity();

    constexpr const size_t vertex_buffer_size = 4;
    RHIBuffer* vertex_buffers[vertex_buffer_size];

    vertex_buffers[0] = uploader.send_buffer(RHIStagingBufferContext(
        RHIBufferUsageFlags::Vertex, submesh.positions.size(), submesh.positions.data()));

    vertex_buffers[1] = uploader.send_buffer(RHIStagingBufferContext(
        RHIBufferUsageFlags::Vertex, submesh.normals.size(), submesh.normals.data()));

    vertex_buffers[2] = uploader.send_buffer(RHIStagingBufferContext(
        RHIBufferUsageFlags::Vertex, submesh.uv_textures.size(), submesh.uv_textures.data()));

    vertex_buffers[3] = uploader.send_buffer(RHIStagingBufferContext(
        RHIBufferUsageFlags::Vertex, submesh.tangents.size(), submesh.tangents.data()));

    FixedArray<TextureBuffer*, 2> textures = {
        &submesh.material.diffuse_texture,
        &submesh.material.normal_texture,
    };

    for (size_t texture = 0; texture < textures.size(); texture++) {
        TextureBuffer& texture_buffer = *textures[texture];
        if (texture_buffer.data.empty()) {
            item.textures.append(nullptr);
            item.texture_views.append(nullptr);
            item.samplers.append(nullptr);
            continue;
        }

        RHITextureDescription tex_desc = {};
        tex_desc.format = texture_buffer.format;
        tex_desc.memory_usage = RHIMemoryUsage::Device;
        tex_desc.usage = RHITextureUsageFlags::Sampled;
        tex_desc.sharing_mode = RHISharingMode::Shared;
        tex_desc.width = texture_buffer.width;
        tex_desc.height = texture_buffer.height;
        tex_desc.mip_levels = Math::floor(Math::log2(Math::max(tex_desc.width, tex_desc.height))) + 1;

        item.vertex_buffers = Array<RHIBuffer*>(vertex_buffers, vertex_buffer_size);
        item.textures.append(uploader.send_texture(RHIStagingBufferContext(
                                                       RHIBufferUsageFlags::Storage, texture_buffer.data.size(), texture_buffer.data.data()),
                                                   tex_desc));

        item.texture_views.append(device->create_texture_view(RHITextureViewDescription{
            .texture = item.textures[texture],
            .format = tex_desc.format,
            .dimension = RHITextureDimension::Dim2D,
            .mip_levels = tex_desc.mip_levels,
        }));

        item.samplers.append(device->create_sampler(RHISamplerDescription{
            .max_lod = static_cast<float32>(tex_desc.mip_levels),
        }));
    }

    item.index_buffer = uploader.send_buffer(
        RHIStagingBufferContext(RHIBufferUsageFlags::Index, ArrayView(submesh.indices)));

    item.index_count = submesh.indices.size();
    return item;
}

}  //namespace licht
