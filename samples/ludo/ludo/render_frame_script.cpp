#include "render_frame_script.hpp"

#include "licht/core/containers/array_view.hpp"
#include "licht/core/containers/index_range.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/math/math.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/core/math/quaternion.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/math/vector4.hpp"
#include "licht/core/memory/default_allocator.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/display.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/engine/project_settings.hpp"
#include "licht/renderer/mesh/static_mesh.hpp"
#include "licht/renderer/mesh/static_mesh_loader.hpp"
#include "licht/renderer/render_item.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/device_memory_uploader.hpp"
#include "licht/rhi/graphics_pipeline.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/rhi_module.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/sampler.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi/texture_pool.hpp"
#include "licht/rhi/texture_view.hpp"
#include "licht/scene/camera.hpp"
#include "ludo_types.hpp"
#include "material_graphics_pipeline.hpp"

namespace licht {

RenderFrameScript::RenderFrameScript(Camera* camera)
    : window_handle_(Display::InvalidWindowHandle)
    , device_(nullptr)
    , framebuffers_(3)
    , cmd_allocator_(nullptr)
    , pause_(false)
    , camera_(camera) {
}

void RenderFrameScript::on_startup() {
    ProjectSettings& project_settings = ProjectSettings::get_instance();
    ModuleRegistry& registry = ModuleRegistry::get_instance();
    RHIModule* module = registry.get_module<RHIModule>(RHIModule::ModuleName);
    String projectdir(project_settings.get_name("projectdir"));

    FileSystem& file_system = FileSystem::get_platform();

    device_ = module->get_device();

    window_handle_ = module->get_window_handle();
    LLOG_FATAL_WHEN(
        !Display::get_default().is_valid(window_handle_),
        "[RHIModule]",
        "Failed to retrieve a valid window handle. Ensure a window is created before initializing the RHI Module.");

    renderer_ = new_ref<RenderContext>();
    material_graphics_pipeline_ = new_ref<MaterialGraphicsPipeline>();

    graphics_queue_ = device_->get_graphics_queue();
    present_queue_ = device_->get_queue_if([&](RHICommandQueueRef command_queue) -> bool {
        return command_queue->is_graphics_type() && command_queue->is_present_mode() && graphics_queue_ != present_queue_;
    });

    cmd_allocator_ = device_->create_command_allocator({
        .command_queue = graphics_queue_,
        .count = renderer_->get_frame_count(),
    });

    // Setup renderer.
    renderer_->set_graphics_queue(graphics_queue_);
    renderer_->set_present_queue(present_queue_);
    renderer_->set_window_handle(window_handle_);
    renderer_->set_command_allocator(cmd_allocator_);
    renderer_->set_on_reset([this]() -> void { reset(); });
    renderer_->startup();

    float32 width = renderer_->get_swapchain()->get_width();
    float32 height = renderer_->get_swapchain()->get_height();

    // Pools
    buffer_pool_ = device_->create_buffer_pool();
    buffer_pool_->initialize_pool(&DefaultAllocator::get_instance(), 64);

    texture_pool_ = device_->create_texture_pool();
    texture_pool_->initialize_pool(&DefaultAllocator::get_instance(), 64);

    // Model
    String model_asset_path = projectdir;
    model_asset_path.append("/assets/models/Sponza/glTF/Sponza.gltf");
    Array<StaticMesh> model = gltf_static_meshes_load(model_asset_path);

    depth_texture_ = texture_pool_->create_texture({
        .format = RHIFormat::D24S8,
        .usage = RHITextureUsageFlags::DepthStencilAttachment,
        .sharing_mode = RHISharingMode::Private,
        .memory_usage = RHIMemoryUsage::Device,
        .width = width,
        .height = height,
    });

    rhi_transition_texture(device_,
                           RHITextureLayoutTransition(depth_texture_,
                                                      RHITextureLayout::Undefined,
                                                      RHITextureLayout::DepthStencilAttachment),
                           graphics_queue_);

    depth_texture_view_ = device_->create_texture_view({
        .texture = depth_texture_,
        .format = depth_texture_->get_description().format,
    });

    material_graphics_pipeline_->initialize(device_, renderer_, buffer_pool_, texture_pool_);

    // -- Framebuffers --
    framebuffers_.reserve(renderer_->get_swapchain()->get_texture_views().size());
    for (RHITextureView* texture : renderer_->get_swapchain()->get_texture_views()) {
        RHIFramebufferDescription description = {
            .render_pass = material_graphics_pipeline_->get_render_pass(),
            .attachments = {texture, depth_texture_view_},
            .width = width,
            .height = height,
            .layers = 1,
        };

        RHIFramebuffer* framebuffer = device_->create_framebuffer(description);
        framebuffers_.append(framebuffer);
    }

    // Device objects --
    RHIDeviceMemoryUploader uploader(device_, buffer_pool_, texture_pool_);

    for (StaticMesh& mesh : model) {
        for (StaticSubMesh& submesh : mesh.get_submeshes()) {
            RenderItem item;

            item.model_constant.model = Matrix4f::identity();
            item.model_constant.model = Matrix4f::scale(item.model_constant.model, Vector3f(.009f));

            constexpr size_t vertex_buffer_size = 3;
            RHIBuffer* vertex_buffers[vertex_buffer_size];

            vertex_buffers[0] = uploader.send_buffer(RHIStagingBufferContext(
                RHIBufferUsageFlags::Vertex, submesh.positions.size(), submesh.positions.data()));

            vertex_buffers[1] = uploader.send_buffer(RHIStagingBufferContext(
                RHIBufferUsageFlags::Vertex, submesh.normals.size(), submesh.normals.data()));

            vertex_buffers[2] = uploader.send_buffer(RHIStagingBufferContext(
                RHIBufferUsageFlags::Vertex, submesh.uv_textures.size(), submesh.uv_textures.data()));

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

                item.texture_views.append(device_->create_texture_view(RHITextureViewDescription{
                    .texture = item.textures[texture],
                    .format = tex_desc.format,
                    .dimension = RHITextureDimension::Dim2D,
                    .mip_levels = tex_desc.mip_levels,
                }));

                item.samplers.append(device_->create_sampler(RHISamplerDescription{
                    .max_lod = static_cast<float32>(tex_desc.mip_levels),
                }));
            }

            item.index_buffer = uploader.send_buffer(
                RHIStagingBufferContext(RHIBufferUsageFlags::Index, ArrayView(submesh.indices)));

            item.index_count = submesh.indices.size();
            packet_.items.append(item);
        }
    }

    packet_.light = RenderPunctualLight{
        .position = Vector3f(0.0f, 10.0f, 0.0f),
        .color = Vector3f(1.0f, 0.5f, 0.2f),
    };

    uploader.upload(graphics_queue_);

    material_graphics_pipeline_->initialize_shader_resource_pool(packet_.items.size());
    material_graphics_pipeline_->compile(packet_);
}

void RenderFrameScript::on_tick(float64 delta_time) {
    if (pause_) {
        return;
    }

    renderer_->begin_frame();
    {
        RHICommandBuffer* cmd = renderer_->get_current_command_buffer();
        float32 width = static_cast<float32>(renderer_->get_swapchain()->get_width());
        float32 height = static_cast<float32>(renderer_->get_swapchain()->get_height());
        Rect2D area = {
            .x = 0.0f,
            .y = 0.0f,
            .width = width,
            .height = height,
        };

        RHIRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.render_pass = material_graphics_pipeline_->get_render_pass();
        render_pass_begin_info.framebuffer = framebuffers_[renderer_->get_frame_index()];
        render_pass_begin_info.area = area;
        render_pass_begin_info.color = Vector4f(0.01f, 0.01f, 0.01f, 1.0f);

        cmd->begin_render_pass(render_pass_begin_info);
        {
            RHIGraphicsPipeline* graphics_pipeline = material_graphics_pipeline_->get_graphics_pipeline_handle();

            cmd->bind_graphics_pipeline(graphics_pipeline);

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

            cmd->set_viewports(&viewport, 1);
            cmd->set_scissors(&scissor, 1);

            // Render Packet
            for (RenderItem& item : packet_.items) {
                RHIShaderResourceGroup* global_group = material_graphics_pipeline_
                                                           ->get_global_shader_resource_pool()
                                                           ->get_group(renderer_->get_current_frame());

                cmd->bind_shader_resource_group(graphics_pipeline, {global_group}, 0);

                cmd->set_shader_constants(graphics_pipeline, RHIShaderConstants(&item.model_constant,
                                                                                sizeof(RenderModelConstant),
                                                                                0,
                                                                                RHIShaderStage::Vertex));

                RHIShaderResourceGroup* shader_group = item.shader_groups[renderer_->get_current_frame()];

                cmd->bind_vertex_buffers(item.vertex_buffers);
                cmd->bind_index_buffer(item.index_buffer);

                cmd->bind_shader_resource_group(graphics_pipeline, {shader_group}, 1);

                RHIDrawIndexedCommand draw_indexed_command = {};
                draw_indexed_command.index_count = item.index_count;
                draw_indexed_command.instance_count = 1;

                cmd->draw(draw_indexed_command);
            }
        }
        cmd->end_render_pass();

        update_uniform(delta_time);
    }
    renderer_->end_frame();
}

void RenderFrameScript::update_uniform(const float64 delta_time) {
    UniformBufferObject ubo;

    static float32 rotation_x = 0.0f;
    static float32 rotation_y = 0.0f;

    rotation_x += delta_time * 30.0f;
    rotation_y += delta_time * 45.0f;

    ubo.view = camera_->view;

    float32 aspect_ratio = renderer_->get_swapchain()->get_width() / static_cast<float32>(renderer_->get_swapchain()->get_height());
    ubo.proj = Matrix4f::perspective(Math::radians(75.0f), aspect_ratio, 0.1f, 10000.0f);

    ubo.view_proj = ubo.proj * ubo.view;

    ubo.eye_position = camera_->position;

    material_graphics_pipeline_->update(ubo);
    material_graphics_pipeline_->update(packet_.light);
}

void RenderFrameScript::reset() {
    device_->destroy_texture_view(depth_texture_view_);
    texture_pool_->destroy_texture(depth_texture_);

    RHITextureDescription depth_texture_desc = {};
    depth_texture_desc.format = RHIFormat::D24S8;
    depth_texture_desc.usage = RHITextureUsageFlags::DepthStencilAttachment;
    depth_texture_desc.sharing_mode = RHISharingMode::Private,
    depth_texture_desc.memory_usage = RHIMemoryUsage::Device,
    depth_texture_desc.width = renderer_->get_swapchain()->get_width();
    depth_texture_desc.height = renderer_->get_swapchain()->get_height();
    depth_texture_ = texture_pool_->create_texture(depth_texture_desc);

    RHITextureViewDescription depth_texture_view_desc = {};
    depth_texture_view_desc.texture = depth_texture_;
    depth_texture_view_desc.format = depth_texture_->get_description().format;
    depth_texture_view_ = device_->create_texture_view(depth_texture_view_desc);

    // Destroy all existing framebuffers
    for (RHIFramebuffer* framebuffer : framebuffers_) {
        device_->destroy_framebuffer(framebuffer);
    }
    framebuffers_.clear();

    // Create new framebuffers
    framebuffers_.reserve(renderer_->get_swapchain()->get_texture_views().size());
    for (RHITextureView* texture_view : renderer_->get_swapchain()->get_texture_views()) {
        RHIFramebufferDescription description = {};
        description.height = renderer_->get_swapchain()->get_height();
        description.width = renderer_->get_swapchain()->get_width();
        description.render_pass = material_graphics_pipeline_->get_render_pass();
        description.attachments = {texture_view, depth_texture_view_};
        description.layers = 1;

        RHIFramebuffer* framebuffer = device_->create_framebuffer(description);
        framebuffers_.append(framebuffer);
    }
}

void RenderFrameScript::on_shutdown() {
    renderer_->shutdown();

    device_->destroy_command_allocator(cmd_allocator_);

    for (RHIFramebuffer* framebuffer : framebuffers_) {
        device_->destroy_framebuffer(framebuffer);
    }
    framebuffers_.clear();

    material_graphics_pipeline_->destroy();

    device_->destroy_texture_view(depth_texture_view_);

    for (RenderItem& item : packet_.items) {
        for (size_t i : IndexRange(0, item.samplers.size())) {
            if (item.samplers[i]) {
                device_->destroy_sampler(item.samplers[i]);
                device_->destroy_texture_view(item.texture_views[i]);
            }
        }
    }

    texture_pool_->dispose();

    buffer_pool_->dispose();

    device_->destroy_swapchain(renderer_->get_swapchain());
}

void RenderFrameScript::update_resized(const uint32 width, const uint32 height) {
    renderer_->update_resized(width, height);
}

void RenderFrameScript::pause() {
    pause_ = true;
}

void RenderFrameScript::unpause() {
    pause_ = false;
}

}  // namespace licht