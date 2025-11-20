#include "ui_graphics_pipeline.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/io/file_handle.hpp"
#include "licht/renderer/render_context.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "ludo_types.hpp"

namespace licht {

void UIGraphicsPipeline::initialize(const SharedRef<RHIDevice>& device,
                                    const SharedRef<RenderContext>& render_context) {
    
    render_context_ = render_context;
    device_ = device;

    // Render Pass.
    RHIColorAttachmentDescription render_pass_color_attachment = {};
    render_pass_color_attachment.format = render_context_->get_swapchain()->get_format();
    render_pass_color_attachment.load_op = RHIAttachmentLoadOp::Load_OP_Load;
    render_pass_color_attachment.initial_layout = RHITextureLayout::ColorAttachment;
    render_pass_color_attachment.final_layout = RHITextureLayout::Present;

    render_pass_ = device_->create_render_pass({
        .color_attachment_decriptions = {render_pass_color_attachment},
        .deph_attachement_description = Option<RHIDepthAttachementDescription>::none(),
    });

    RHIShaderResourceBinding ubo_binding(0, RHIShaderResourceType::Uniform, RHIShaderStage::Fragment | RHIShaderStage::Vertex);

    global_bindings_ = {ubo_binding};
    global_shader_resource_layout_ = device_->create_shader_resource_layout(global_bindings_);

    create_pipeline_internal();
}

void UIGraphicsPipeline::initialize_shader_resource_pool(size_t item_count) {
    size_t max_global_count = render_context_->get_frame_count();
    size_t max_item_count = render_context_->get_frame_count() * item_count;

    global_shader_resource_pool_ = device_->create_shader_resource_pool(max_global_count, global_bindings_);

    uniform_buffers_.reserve(max_global_count);

    for (uint32 frame = 0; frame < max_global_count; frame++) {
        uniform_buffers_.append(render_context_->get_buffer_pool()->create_buffer(
            RHIBufferDescription(sizeof(UniformBufferObject),
                                 RHIBufferUsageFlags::Uniform,
                                 RHIMemoryUsage::Host,
                                 RHISharingMode::Private)));
    }

    for (size_t i = 0; i < max_global_count; i++) {
        global_shader_resource_pool_->allocate_group(global_shader_resource_layout_);
    }

    for (size_t i = 0; i < max_item_count; i++) {
        // texture_shader_resource_pool_->allocate_group(texture_shader_resource_layout_);
    }
}

void UIGraphicsPipeline::reload() {
    LLOG_INFO("[UIGraphicsPipeline]", "Reloading UI shaders and rebuilding pipeline...");

    destroy_pipeline_internal();
    create_pipeline_internal();

    LLOG_INFO("[UIGraphicsPipeline]", "UI Pipeline successfully reloaded.");
}

void UIGraphicsPipeline::update(const UniformBufferObject& ubo) {
    uniform_buffers_[render_context_->get_current_frame()]->update(&ubo, sizeof(UniformBufferObject));
}

void UIGraphicsPipeline::create_pipeline_internal() {
     // Bindings and attributes.
    RHIVertexBindingDescription position_input_binding_description(
        0, sizeof(Vector3f), RHIVertexInputRate::Vertex);
    RHIVertexAttributeDescription position_attribute_description(
        0, 0, RHIFormat::RGB32Float);

    RHIVertexBindingDescription color_input_binding_description(
        1, sizeof(Vector4f), RHIVertexInputRate::Vertex);
    RHIVertexAttributeDescription color_attribute_description(
        1, 1, RHIFormat::RGBA32Float);

    RHIVertexBindingDescription texture_uv_input_binding_description(
        2, sizeof(Vector2f), RHIVertexInputRate::Vertex);
    RHIVertexAttributeDescription texture_uv_attribute_description(
        2, 2, RHIFormat::RG32Float);

    // Fetch size.
    float32 width = render_context_->get_swapchain()->get_width();
    float32 height = render_context_->get_swapchain()->get_height();

    // Load shaders binary codes.
    FileSystem& file_system = FileSystem::get_platform();
    FileHandleResult vertex_file_open_error = file_system.open_read("ludo.ui.vert.spv");
    LCHECK(vertex_file_open_error.has_value());

    SharedRef<FileHandle> vertex_file_handle = vertex_file_open_error.value();
    SPIRVShader vertex_shader(vertex_file_handle->read_all_bytes());

    FileHandleResult fragment_file_open_error = file_system.open_read("ludo.ui.frag.spv");
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
                {position_input_binding_description, color_input_binding_description, texture_uv_input_binding_description},
                {position_attribute_description, color_attribute_description, texture_uv_attribute_description})
            .with_viewport(viewport, scissor)
            .with_shader_resource_group_layout({
                global_shader_resource_layout_,   // Set 0
            })
            .with_shader_constant_ranges({
                RHIShaderConstantRange(RHIShaderStage::Vertex, 0, sizeof(RenderModelConstant)),
            })
            .with_cull_mode(RHICullModeFlags::None)
            .build());
}

void UIGraphicsPipeline::destroy_pipeline_internal() {
    device_->destroy_graphics_pipeline(graphics_pipeline_);
}

void UIGraphicsPipeline::destroy() {
    destroy_pipeline_internal();

    device_->destroy_shader_resource_layout(global_shader_resource_layout_);
    device_->destroy_shader_resource_pool(global_shader_resource_pool_);

    device_->destroy_render_pass(render_pass_);
}

void UIGraphicsPipeline::compile(DrawPacket& packet) {
    for (uint32 frame = 0; frame < render_context_->get_frame_count(); frame++) {
        for (DrawItem& item : packet.items) {
            
        }
    }
}

}  //namespace licht