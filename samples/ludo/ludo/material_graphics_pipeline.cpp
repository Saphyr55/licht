#include "material_graphics_pipeline.hpp"
#include "licht/core/function/functors.hpp"
#include "licht/core/io/file_handle.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/renderer/render_item.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/buffer_pool.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/render_pass.hpp"
#include "ludo_types.hpp"

namespace licht {

void MaterialGraphicsPipeline::initiliaze(const SharedRef<RHIDevice>& device,
                                          const SharedRef<Renderer>& renderer,
                                          const SharedRef<RHIBufferPool>& buffer_pool) {
    renderer_ = renderer;
    device_ = device;
    buffer_pool_ = buffer_pool;

    FileSystem& file_system = FileSystem::get_platform();

    float32 width = renderer_->get_swapchain()->get_width();
    float32 height = renderer_->get_swapchain()->get_height();

    // Render Pass.
    RHIColorAttachmentDescription render_pass_color_attachment = {};
    render_pass_color_attachment.format = renderer_->get_swapchain()->get_format();

    render_pass_ = device_->create_render_pass({
        .attachment_decriptions = {render_pass_color_attachment},
        .deph_attachement_description = RHIDepthAttachementDescription(RHIFormat::D24S8),
    });

    ubo_binding_ = RHIShaderResourceBinding(0,
                                            RHIShaderResourceType::Uniform,
                                            RHIShaderStage::Fragment | RHIShaderStage::Vertex);

    sampler_binding_ = RHIShaderResourceBinding(1,
                                                RHIShaderResourceType::Sampler,
                                                RHIShaderStage::Fragment);

    shader_resource_layout_ = device_->create_shader_resource_layout({ubo_binding_, sampler_binding_});

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

    // Load shaders binary codes.
    FileHandleResult vertex_file_open_error = file_system.open_read("shaders/main.vert.spv");
    LCHECK(vertex_file_open_error.has_value());

    SharedRef<FileHandle> vertex_file_handle = vertex_file_open_error.value();
    SPIRVShader vertex_shader(vertex_file_handle->read_all_bytes());

    FileHandleResult fragment_file_open_error = file_system.open_read("shaders/main.frag.spv");
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

    // -- Graphics Pipeline --
    graphics_pipeline_ = device_->create_graphics_pipeline(
        RHIGraphicsPipeline::description_builder()
            .with_render_pass(render_pass_)
            .with_vertex_shader(vertex_shader, "main")
            .with_fragment_shader(fragment_shader, "main")
            .with_vertex_bindings(
                {position_input_binding_description, normal_input_binding_description, texture_uv_input_binding_description},
                {position_attribute_description, normal_attribute_description, texture_uv_attribute_description})
            .with_viewport(viewport, scissor)
            .with_shader_resource_group_layout(shader_resource_layout_)
            .with_shader_constant_ranges({
                RHIShaderConstantRange(RHIShaderStage::Vertex, 0, sizeof(RenderModelConstant)),
            })
            .with_cull_mode(RHICullModeFlags::Back)
            .build());
}

void MaterialGraphicsPipeline::destroy() {
    device_->destroy_graphics_pipeline(graphics_pipeline_);
    device_->destroy_render_pass(render_pass_);
    uniform_buffers_.clear();
    shader_resource_pool_->dispose();
    device_->destroy_shader_resource_pool(shader_resource_pool_);
}

void MaterialGraphicsPipeline::compile(const Array<RenderPacket>& packets) {
    size_t draw_item_count = licht::reduce(packets.begin(), packets.end(), 0,
                                           [](size_t v, const RenderPacket& packet) -> size_t {
                                               return v + packet.items.size();
                                           });

    size_t max_shader_resource = draw_item_count * renderer_->get_frame_count();
    shader_resource_pool_ = device_->create_shader_resource_pool(max_shader_resource, {ubo_binding_, sampler_binding_});

    uniform_buffers_.reserve(renderer_->get_frame_count());

    for (uint32 frame = 0; frame < renderer_->get_frame_count(); frame++) {
        RHIBufferDescription uniform_buffer_desc(sizeof(UniformBufferObject),
                                                 RHIBufferUsageFlags::Uniform,
                                                 RHIMemoryUsage::Host,
                                                 RHISharingMode::Private);
        uniform_buffers_.append(buffer_pool_->create_buffer(uniform_buffer_desc));
    }

    for (RenderPacket& packet : packets) {
        for (RenderItem& item : packet.items) {
            item.shader_groups.reserve(renderer_->get_frame_count());
            for (uint32 frame = 0; frame < renderer_->get_frame_count(); frame++) {
                RHIShaderResourceGroup* group = shader_resource_pool_->allocate_group(shader_resource_layout_);

                RHIBuffer* uniform_buffer = uniform_buffers_[frame];
                group->set_buffer(RHIWriteBufferResource(
                    ubo_binding_.binding,
                    uniform_buffer,
                    0,
                    uniform_buffer->get_size()));

                group->set_texture_sampler(RHIWriteTextureSamplerResource(
                    sampler_binding_.binding,
                    item.texture_view,
                    item.sampler));

                group->compile();
                item.shader_groups.append(group);
            }
        }
    }
}

void MaterialGraphicsPipeline::update(const UniformBufferObject& ubo) {
    uniform_buffers_[renderer_->get_current_frame()]->update(&ubo, sizeof(UniformBufferObject), 0);
}

}  //namespace licht
