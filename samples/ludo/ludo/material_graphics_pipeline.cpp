#include "material_graphics_pipeline.hpp"
#include "licht/core/io/file_handle.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/renderer/render_item.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/buffer_pool.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/device_memory_uploader.hpp"
#include "ludo_types.hpp"

namespace licht {

void MaterialGraphicsPipeline::initialize(const SharedRef<RHIDevice>& device,
                                          const SharedRef<RenderContext>& renderer,
                                          const SharedRef<RHIBufferPool>& buffer_pool,
                                          const SharedRef<RHITexturePool>& texture_pool) {
    renderer_ = renderer;
    device_ = device;
    buffer_pool_ = buffer_pool;
    texture_pool_ = texture_pool;

    Array<uint8> texture_buffer;
    texture_buffer.resize(4);
    texture_buffer[0] = 255; 
    texture_buffer[1] = 255;
    texture_buffer[2] = 255; 
    texture_buffer[3] = 255;

    RHITextureDescription tex_desc = {};
    tex_desc.format = RHIFormat::RGBA8sRGB;
    tex_desc.memory_usage = RHIMemoryUsage::Device;
    tex_desc.usage = RHITextureUsageFlags::Sampled;
    tex_desc.sharing_mode = RHISharingMode::Shared;
    tex_desc.width = 1;
    tex_desc.height = 1;
    tex_desc.mip_levels = 1;

    RHIDeviceMemoryUploader uploader(device_, buffer_pool_, texture_pool_);
    default_texture_ = uploader.send_texture(RHIStagingBufferContext(
                              RHIBufferUsageFlags::Storage, texture_buffer.size(), texture_buffer.data()),
                          tex_desc);

    default_texture_view_ = device_->create_texture_view({
        .texture = default_texture_,
        .format = tex_desc.format,
        .dimension = RHITextureDimension::Dim2D,
        .mip_levels = tex_desc.mip_levels,
    });

    default_sampler_ = device_->create_sampler({});

    uploader.upload(device_->get_graphics_queue());

    FileSystem& file_system = FileSystem::get_platform();

    // Load shaders binary codes.
    FileHandleResult vertex_file_open_error = file_system.open_read("shaders/ludo.material.vert.spv");
    LCHECK(vertex_file_open_error.has_value());

    SharedRef<FileHandle> vertex_file_handle = vertex_file_open_error.value();
    SPIRVShader vertex_shader(vertex_file_handle->read_all_bytes());

    FileHandleResult fragment_file_open_error = file_system.open_read("shaders/ludo.material.frag.spv");
    LCHECK(fragment_file_open_error.has_value());

    SharedRef<FileHandle> fragment_file_handle = fragment_file_open_error.value();
    SPIRVShader fragment_shader(fragment_file_handle->read_all_bytes());

    // Fetch size.
    float32 width = renderer_->get_swapchain()->get_width();
    float32 height = renderer_->get_swapchain()->get_height();

    // Render Pass.
    RHIColorAttachmentDescription render_pass_color_attachment = {};
    render_pass_color_attachment.format = renderer_->get_swapchain()->get_format();

    render_pass_ = device_->create_render_pass({
        .color_attachment_decriptions = {render_pass_color_attachment},
        .deph_attachement_description = RHIDepthAttachementDescription(RHIFormat::D24S8),
    });

    RHIShaderResourceBinding ubo_binding(0,
                                         RHIShaderResourceType::Uniform,
                                         RHIShaderStage::Fragment | RHIShaderStage::Vertex);

    RHIShaderResourceBinding lights_binding(1,
                                            RHIShaderResourceType::Uniform,
                                            RHIShaderStage::Fragment);

    RHIShaderResourceBinding diffuse_binding(0,
                                             RHIShaderResourceType::Sampler,
                                             RHIShaderStage::Fragment);

    RHIShaderResourceBinding normal_binding(1,
                                            RHIShaderResourceType::Sampler,
                                            RHIShaderStage::Fragment);

    global_bindings_ = {ubo_binding, lights_binding};
    texture_bindings_ = {diffuse_binding, normal_binding};

    global_shader_resource_layout_ = device_->create_shader_resource_layout(global_bindings_);
    texture_shader_resource_layout_ = device_->create_shader_resource_layout(texture_bindings_);

    // Bindings and attributes.
    RHIVertexBindingDescription position_input_binding_description(
        0, sizeof(Vector3f), RHIVertexInputRate::Vertex);

    RHIVertexAttributeDescription position_attribute_description(
        0, 0, RHIFormat::RGB32Float);

    RHIVertexBindingDescription normal_input_binding_description(
        1, sizeof(Vector3f), RHIVertexInputRate::Vertex);

    RHIVertexAttributeDescription normal_attribute_description(
        1, 1, RHIFormat::RGB32Float);

    RHIVertexBindingDescription texture_uv_input_binding_description(
        2, sizeof(Vector2f), RHIVertexInputRate::Vertex);

    RHIVertexAttributeDescription texture_uv_attribute_description(
        2, 2, RHIFormat::RG32Float);

    Rect2D scissor = {
        .x = 0.0f,
        .y = 0.0f,
        .width = width,
        .height = height,
    };

    Viewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = width,
        .height = height,
        .min_depth = 0.0f,
        .max_depth = 1.0f,
    };

    // -- Graphics Pipeline --
    graphics_pipeline_ = device_->create_graphics_pipeline(
        RHIGraphicsPipelineDescriptionBuilder()
            .with_render_pass(render_pass_)
            .with_vertex_shader(vertex_shader, "main")
            .with_fragment_shader(fragment_shader, "main")
            .with_vertex_bindings(
                {position_input_binding_description, normal_input_binding_description, texture_uv_input_binding_description},
                {position_attribute_description, normal_attribute_description, texture_uv_attribute_description})
            .with_viewport(viewport, scissor)
            .with_shader_resource_group_layout({
                global_shader_resource_layout_, // Set 0
                texture_shader_resource_layout_,// Set 1
             })
            .with_shader_constant_ranges({
                RHIShaderConstantRange(RHIShaderStage::Vertex, 0, sizeof(RenderModelConstant)),
            })
            .with_cull_mode(RHICullModeFlags::Back)
            .build());
}

void MaterialGraphicsPipeline::initialize_shader_resource_pool(size_t item_count) {
    size_t max_global_count = renderer_->get_frame_count();
    size_t max_item_count = renderer_->get_frame_count() * item_count;

    global_shader_resource_pool_ = device_->create_shader_resource_pool(max_global_count, global_bindings_);
    texture_shader_resource_pool_ = device_->create_shader_resource_pool(max_item_count, texture_bindings_);

    uniform_buffers_.reserve(max_global_count);
    light_buffers_.reserve(max_global_count);

    for (uint32 frame = 0; frame < max_global_count; frame++) {
        uniform_buffers_.append(buffer_pool_->create_buffer(
            RHIBufferDescription(sizeof(UniformBufferObject),
                                 RHIBufferUsageFlags::Uniform,
                                 RHIMemoryUsage::Host,
                                 RHISharingMode::Private)));

        light_buffers_.append(buffer_pool_->create_buffer(
            RHIBufferDescription(sizeof(RenderPunctualLight),
                                 RHIBufferUsageFlags::Uniform,
                                 RHIMemoryUsage::Host,
                                 RHISharingMode::Private)));
    }

    for (size_t i = 0; i < max_global_count; i++) {
        global_shader_resource_pool_->allocate_group(global_shader_resource_layout_);
    }

    for (size_t i = 0; i < max_item_count; i++) {
        texture_shader_resource_pool_->allocate_group(texture_shader_resource_layout_);
    }
}

void MaterialGraphicsPipeline::destroy() {
    device_->destroy_graphics_pipeline(graphics_pipeline_);
    device_->destroy_render_pass(render_pass_);

    device_->destroy_sampler(default_sampler_);
    device_->destroy_texture_view(default_texture_view_);

    uniform_buffers_.clear();
    light_buffers_.clear();

    if (global_shader_resource_pool_) {
        global_shader_resource_pool_->dispose();
        device_->destroy_shader_resource_pool(global_shader_resource_pool_);
        global_shader_resource_pool_ = nullptr;
    }

    if (texture_shader_resource_pool_) {
        texture_shader_resource_pool_->dispose();
        device_->destroy_shader_resource_pool(texture_shader_resource_pool_);
        texture_shader_resource_pool_ = nullptr;
    }
}

void MaterialGraphicsPipeline::compile(const RenderPacket& packet) {
    if (!global_shader_resource_pool_ || !texture_shader_resource_pool_) {
        LLOG_ERROR("[MaterialGraphicsPipeline]", "Shader resource pools are not initialized.");
        return;
    }

    for (uint32 frame = 0; frame < renderer_->get_frame_count(); frame++) {
        RHIShaderResourceGroup* uniform_group = global_shader_resource_pool_->get_group(frame);

        RHIBuffer* uniform_buffer = uniform_buffers_[frame];
        uniform_group->set_buffer(RHIWriteBufferResource(global_bindings_[0].binding,
                                                        uniform_buffer,
                                                        0,
                                                        uniform_buffer->get_size()));

        RHIBuffer* light_buffer = light_buffers_[frame];
        uniform_group->set_buffer(RHIWriteBufferResource(global_bindings_[1].binding,
                                                        light_buffer,
                                                        0,
                                                        light_buffer->get_size()));

        uniform_group->compile();
    }

    size_t sr_texture_pool_index = 0;
    for (uint32 frame = 0; frame < renderer_->get_frame_count(); frame++) {
        
        for (RenderItem& item : packet.items) {

            RHIShaderResourceGroup* tex_group = texture_shader_resource_pool_->get_group(sr_texture_pool_index++);

            if (item.samplers[0]) {
                tex_group->set_texture_sampler(RHIWriteTextureSamplerResource(texture_bindings_[0].binding,
                                                                              item.texture_views[0],
                                                                              item.samplers[0]));
            } else {
                tex_group->set_texture_sampler(RHIWriteTextureSamplerResource(texture_bindings_[0].binding,
                                                                              default_texture_view_,
                                                                              default_sampler_));
            }

            if (item.samplers[1]) {
                tex_group->set_texture_sampler(RHIWriteTextureSamplerResource(texture_bindings_[1].binding,
                                                                              item.texture_views[1],
                                                                              item.samplers[1]));
            } else {
                tex_group->set_texture_sampler(RHIWriteTextureSamplerResource(texture_bindings_[1].binding,
                                                                              default_texture_view_,
                                                                              default_sampler_));
            }
                
            tex_group->compile();

            if (item.shader_groups.empty()) {
                item.shader_groups.resize(renderer_->get_frame_count());
            }

            item.shader_groups[frame] = tex_group;
        }
    }

}

void MaterialGraphicsPipeline::update(const RenderPunctualLight& light) {
    light_buffers_[renderer_->get_current_frame()]->update(&light, sizeof(RenderPunctualLight), 0);
}

void MaterialGraphicsPipeline::update(const UniformBufferObject& ubo) {
    uniform_buffers_[renderer_->get_current_frame()]->update(&ubo, sizeof(UniformBufferObject), 0);
}

}  //namespace licht
