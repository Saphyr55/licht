#include "material_graphics_pipeline.hpp"
#include "licht/core/io/file_handle.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/buffer_pool.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/device_memory_uploader.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/scene/punctual_light.hpp"
#include "ludo_types.hpp"

namespace licht {

void MaterialGraphicsPipeline::initialize(const SharedRef<RHIDevice>& device,
                                          const SharedRef<RenderContext>& render_context) {
    render_context_ = render_context;
    device_ = device;
    buffer_pool_ = render_context->get_buffer_pool();
    texture_pool_ = render_context->get_texture_pool();

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
    // Remove the default texture by using a sampler array.
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

    // Render Pass.
    RHIColorAttachmentDescription render_pass_color_attachment = {};
    render_pass_color_attachment.format = render_context_->get_swapchain()->get_format();
    render_pass_color_attachment.load_op = RHIAttachmentLoadOp::Load_OP_Clear;
    render_pass_color_attachment.store_op = RHIAttachmentStoreOp::Store_OP_Store;
    render_pass_color_attachment.initial_layout = RHITextureLayout::Undefined;
    render_pass_color_attachment.final_layout = RHITextureLayout::ColorAttachment;
    
    RHIDepthAttachementDescription render_pass_depth_attachment = {};
    render_pass_depth_attachment.format = RHIFormat::D24S8;
    render_pass_depth_attachment.load_op = RHIAttachmentLoadOp::Load_OP_Clear;
    render_pass_depth_attachment.store_op = RHIAttachmentStoreOp::Store_OP_DontCare;

    render_pass_ = device_->create_render_pass({
        .color_attachment_decriptions = {render_pass_color_attachment},
        .deph_attachement_description = render_pass_depth_attachment,
    });

    RHIShaderResourceBinding ubo_binding(0, RHIShaderResourceType::Uniform, RHIShaderStage::Fragment | RHIShaderStage::Vertex);
    RHIShaderResourceBinding lights_binding(1, RHIShaderResourceType::Uniform, RHIShaderStage::Fragment);
    RHIShaderResourceBinding samplers_indices_binding(2, RHIShaderResourceType::Uniform, RHIShaderStage::Fragment);
    RHIShaderResourceBinding samplers_binding(0, RHIShaderResourceType::Sampler, RHIShaderStage::Fragment, sampler_count);

    global_bindings_ = {ubo_binding, lights_binding, samplers_indices_binding};
    texture_bindings_ = {samplers_binding};

    global_shader_resource_layout_ = device_->create_shader_resource_layout(global_bindings_);
    texture_shader_resource_layout_ = device_->create_shader_resource_layout(texture_bindings_);

    create_pipeline_internal();
}

void MaterialGraphicsPipeline::initialize_shader_resource_pool(size_t item_count) {
    size_t max_global_count = render_context_->get_frame_count();
    size_t max_item_count = render_context_->get_frame_count() * item_count;

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
            RHIBufferDescription(sizeof(PunctualLight),
                                 RHIBufferUsageFlags::Uniform,
                                 RHIMemoryUsage::Host,
                                 RHISharingMode::Private)));

        RHIBuffer* sampler_indices_buffer = buffer_pool_->create_buffer(
            RHIBufferDescription(sizeof(SamplerIndices),
                                 RHIBufferUsageFlags::Uniform,
                                 RHIMemoryUsage::Host,
                                 RHISharingMode::Private));

        sampler_indices_buffers_.append(sampler_indices_buffer);
    }

    for (size_t i = 0; i < max_global_count; i++) {
        global_shader_resource_pool_->allocate_group(global_shader_resource_layout_);
    }

    for (size_t i = 0; i < max_item_count; i++) {
        texture_shader_resource_pool_->allocate_group(texture_shader_resource_layout_);
    }
}

void MaterialGraphicsPipeline::reload() {
    LLOG_INFO("[MaterialGraphicsPipeline]", "Reloading material shaders and rebuilding pipeline...");

    destroy_pipeline_internal();
    create_pipeline_internal();

    LLOG_INFO("[MaterialGraphicsPipeline]", "Material pipeline successfully reloaded.");
}

void MaterialGraphicsPipeline::destroy() {
    destroy_pipeline_internal();

    device_->destroy_shader_resource_layout(global_shader_resource_layout_);
    device_->destroy_shader_resource_layout(texture_shader_resource_layout_);

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

void MaterialGraphicsPipeline::compile(const DrawPacket& packet) {
    if (!global_shader_resource_pool_ || !texture_shader_resource_pool_) {
        LLOG_ERROR("[MaterialGraphicsPipeline]", "Shader resource pools are not initialized.");
        return;
    }

    for (uint32 frame = 0; frame < render_context_->get_frame_count(); frame++) {
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

        RHIBuffer* sampler_indices = sampler_indices_buffers_[frame];
        uniform_group->set_buffer(RHIWriteBufferResource(global_bindings_[2].binding,
                                                         sampler_indices,
                                                         0,
                                                         sampler_indices->get_size()));

        uniform_group->compile();
    }

    size_t sr_texture_pool_index = 0;

    for (uint32 frame = 0; frame < render_context_->get_frame_count(); frame++) {
        for (DrawItem& item : packet.items) {
            RHIShaderResourceGroup* tex_group = texture_shader_resource_pool_->get_group(sr_texture_pool_index++);

            uint32 samplers_binding = texture_bindings_[0].binding;

            RHITextureView* texture_view = item.texture_views[sampler_indices_.diffuse_index] ? item.texture_views[sampler_indices_.diffuse_index] : default_texture_view_;
            RHISampler* sampler = item.samplers[sampler_indices_.diffuse_index] ? item.samplers[sampler_indices_.diffuse_index] : default_sampler_;

            RHIWriteTextureSamplerResource write(samplers_binding, texture_view, sampler, sampler_indices_.diffuse_index);

            tex_group->set_texture_sampler(write);

            texture_view = item.texture_views[sampler_indices_.normal_index] ? item.texture_views[sampler_indices_.normal_index] : default_texture_view_;
            sampler = item.samplers[sampler_indices_.normal_index] ? item.samplers[sampler_indices_.normal_index] : default_sampler_;

            write = RHIWriteTextureSamplerResource(samplers_binding, texture_view, sampler, sampler_indices_.normal_index);

            tex_group->set_texture_sampler(write);

            tex_group->compile();

            if (item.shader_groups.empty()) {
                item.shader_groups.resize(render_context_->get_frame_count());
            }

            item.shader_groups[frame] = tex_group;
        }
    }
}

void MaterialGraphicsPipeline::update(const PunctualLight& light) {
    light_buffers_[render_context_->get_current_frame()]->update(&light, sizeof(PunctualLight));
}

void MaterialGraphicsPipeline::update(const UniformBufferObject& ubo) {
    uniform_buffers_[render_context_->get_current_frame()]->update(&ubo, sizeof(UniformBufferObject));
    sampler_indices_buffers_[render_context_->get_current_frame()]->update(&sampler_indices_, sizeof(SamplerIndices));
}

void MaterialGraphicsPipeline::destroy_pipeline_internal() {
    device_->destroy_graphics_pipeline(graphics_pipeline_);
}

void MaterialGraphicsPipeline::create_pipeline_internal() {
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

    RHIVertexBindingDescription tangent_input_binding_description(
        3, sizeof(Vector4f), RHIVertexInputRate::Vertex);
    RHIVertexAttributeDescription tangent_attribute_description(
        3, 3, RHIFormat::RGBA32Float);

    // Fetch size.
    float32 width = render_context_->get_swapchain()->get_width();
    float32 height = render_context_->get_swapchain()->get_height();

    // Load shaders binary codes.
    FileSystem& file_system = FileSystem::get_platform();

    FileHandleResult vertex_file_open_error = file_system.open_read("ludo.material.vert.spv");
    LCHECK(vertex_file_open_error.has_value());

    SharedRef<FileHandle> vertex_file_handle = vertex_file_open_error.value();
    SPIRVShader vertex_shader(vertex_file_handle->read_all_bytes());

    FileHandleResult fragment_file_open_error = file_system.open_read("ludo.material.frag.spv");
    LCHECK(fragment_file_open_error.has_value());

    SharedRef<FileHandle> fragment_file_handle = fragment_file_open_error.value();
    SPIRVShader fragment_shader(fragment_file_handle->read_all_bytes());

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

    graphics_pipeline_ = device_->create_graphics_pipeline(
        RHIGraphicsPipelineDescriptionBuilder()
            .with_render_pass(render_pass_)
            .with_vertex_shader(vertex_shader, "main")
            .with_fragment_shader(fragment_shader, "main")
            .with_vertex_bindings(
                {position_input_binding_description, normal_input_binding_description, texture_uv_input_binding_description, tangent_input_binding_description},
                {position_attribute_description, normal_attribute_description, texture_uv_attribute_description, tangent_attribute_description})
            .with_viewport(viewport, scissor)
            .with_shader_resource_group_layout({
                global_shader_resource_layout_,   // Set 0
                texture_shader_resource_layout_,  // Set 1
            })
            .with_shader_constant_ranges({
                RHIShaderConstantRange(RHIShaderStage::Vertex, 0, sizeof(RenderModelConstant)),
            })
            .with_cull_mode(RHICullModeFlags::Back)
            .build());
}

}  //namespace licht
