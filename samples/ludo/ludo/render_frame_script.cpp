#include "render_frame_script.hpp"

#include "image.hpp"

#include "licht/core/containers/array_view.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/io/file_handle.hpp"
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
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/device_memory_uploader.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/rhi_module.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/sampler.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi/texture_pool.hpp"
#include "licht/rhi/texture_view.hpp"
#include "licht/scene/camera.hpp"

namespace licht {

RenderFrameScript::RenderFrameScript(Camera* camera)
    : window_handle_(Display::InvalidWindowHandle)
    , device_(nullptr)
    , render_pass_(nullptr)
    , graphics_pipeline_(nullptr)
    , framebuffers_(3)
    , cmd_allocator_(nullptr)
    , pause_(false)
    , camera_(camera)
    , shader_resource_group_layout_(nullptr) {
}

void RenderFrameScript::on_startup() {
    // RHI Module.
    ProjectSettings& project_settings = ProjectSettings::get_instance();
    ModuleRegistry& registry = ModuleRegistry::get_instance();
    RHIModule* module = registry.get_module<RHIModule>(RHIModule::ModuleName);
    String projectdir(project_settings.get_name("projectdir"));

    // Platform file system.
    FileSystem& file_system = FileSystem::get_platform();

    // Device.
    device_ = module->get_device();

    // Window Handle.
    window_handle_ = module->get_window_handle();
    LLOG_FATAL_WHEN(
        !Display::get_default().is_valid(window_handle_),
        "[RHIModule]",
        "Failed to retrieve a valid window handle. Ensure a window is created before initializing the RHI Module.");

    // Renderer
    renderer_ = new_ref<Renderer>();

    // Queues.
    const Array<RHICommandQueueRef>& command_queues = device_->get_command_queues();

    graphics_queue_ = *command_queues.get_if([](RHICommandQueueRef command_queue) {
        return command_queue->is_graphics_type();
    });

    present_queue_ = *command_queues.get_if([&](RHICommandQueueRef command_queue) {
        return command_queue->is_graphics_type() && command_queue->is_present_mode() && graphics_queue_ != present_queue_;
    });

    // Command Allocators
    cmd_allocator_ = device_->create_command_allocator({
        .command_queue = graphics_queue_,
        .count = renderer_->get_frame_count(),
    });

    // Setup renderer.
    renderer_->set_graphics_queue(graphics_queue_);
    renderer_->set_present_queue(present_queue_);
    renderer_->set_window_handle(window_handle_);
    renderer_->set_command_allocator(cmd_allocator_);
    renderer_->set_on_reset([this]() -> void {
        reset();
    });
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

    // Render Pass.
    RHIColorAttachmentDescription render_pass_color_attachment = {};
    render_pass_color_attachment.format = renderer_->get_swapchain()->get_format();

    render_pass_ = device_->create_render_pass({.attachment_decriptions = {render_pass_color_attachment},
                                                .deph_attachement_description = RHIDepthAttachementDescription(RHIFormat::D24S8)});

    RHIShaderResourceBinding ubo_binding(
        0, RHIShaderResourceType::Uniform, RHIShaderStage::Fragment | RHIShaderStage::Vertex, 1);

    sampler_binding_ = RHIShaderResourceBinding(
        1, RHIShaderResourceType::Sampler, RHIShaderStage::Fragment);

    Array<RHIShaderResourceBinding> shader_resource_binding = {ubo_binding, sampler_binding_};
    shader_resource_group_layout_ = device_->create_shader_resource_layout(shader_resource_binding);

    // Bindings and attributes.
    RHIVertexBindingDescription position_input_binding_description(
        0, sizeof(Vector3f), RHIVertexInputRate::Vertex);

    RHIVertexAttributeDescription position_attribute_description(
        0, 0, RHIFormat::RGB32Float);

    RHIVertexBindingDescription texture_uv_input_binding_description(
        1, sizeof(Vector2f), RHIVertexInputRate::Vertex);

    RHIVertexAttributeDescription texture_uv_attribute_description(
        1, 1, RHIFormat::RG32Float);

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
        .width = width,
        .height = height,
    };

    Viewport viewport = {
        .width = width,
        .height = height,
    };

    // -- Graphics Pipeline --
    graphics_pipeline_ = device_->create_graphics_pipeline(
        RHIGraphicsPipeline::description_builder()
            .with_render_pass(render_pass_)
            .with_vertex_shader(vertex_shader, "main")
            .with_fragment_shader(fragment_shader, "main")
            .with_vertex_bindings(
                {position_input_binding_description, texture_uv_input_binding_description},
                {position_attribute_description, texture_uv_attribute_description})
            .with_viewport(viewport, scissor)
            .with_shader_resource_group_layout(shader_resource_group_layout_)
            .with_cull_mode(RHICullModeFlags::Back)
            .build());

    depth_texture_ = texture_pool_->create_texture({
        .format = RHIFormat::D24S8,
        .usage = RHITextureUsageFlags::DepthStencilAttachment,
        .width = width,
        .height = height,
    });

    rhi_transition_texture(device_,
                           RHITextureBarrier(depth_texture_,
                                             RHITextureLayout::Undefined,
                                             RHITextureLayout::DepthStencilAttachment),
                           graphics_queue_);

    depth_texture_view_ = device_->create_texture_view({
        .texture = depth_texture_,
        .format = depth_texture_->get_description().format,
    });

    // -- Framebuffers --
    framebuffers_.reserve(renderer_->get_swapchain()->get_texture_views().size());
    for (RHITextureView* texture : renderer_->get_swapchain()->get_texture_views()) {
        RHIFramebufferDescription description = {
            .render_pass = render_pass_,
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

    RenderMesh render_mesh;

    for (StaticMesh& mesh : model) {
        for (StaticSubMesh& submesh : mesh.get_submeshes()) {
            RenderSubMesh render_submesh;
            RHIBuffer* vertex_buffers[2];

            vertex_buffers[0] = uploader.send_buffer(RHIStagingBufferContext(
                RHIBufferUsageFlags::Vertex, submesh.positions.size(), submesh.positions.data()));

            vertex_buffers[1] = uploader.send_buffer(RHIStagingBufferContext(
                RHIBufferUsageFlags::Vertex, submesh.uv_textures.size(), submesh.uv_textures.data()));

            RHITextureDescription tex_desc;
            tex_desc.format = submesh.material.diffuse_texture.format;
            tex_desc.memory_usage = RHIMemoryUsage::Device;
            tex_desc.usage = RHITextureUsageFlags::Sampled;
            tex_desc.sharing_mode = RHISharingMode::Shared;
            tex_desc.width = submesh.material.diffuse_texture.width;
            tex_desc.height = submesh.material.diffuse_texture.height;

            render_submesh.vertex_buffers = Array<RHIBuffer*>(vertex_buffers, 2);
            render_submesh.texture = uploader.send_texture(RHIStagingBufferContext(
                                                               RHIBufferUsageFlags::Storage,
                                                               submesh.material.diffuse_texture.data.size(),
                                                               submesh.material.diffuse_texture.data.data()),
                                                           tex_desc);

            render_submesh.texture_view = device_->create_texture_view(RHITextureViewDescription{
                .texture = render_submesh.texture,
                .format = tex_desc.format,
                .dimension = RHITextureDimension::Dim2D,
            });

            render_submesh.sampler = device_->create_sampler(RHISamplerDescription{});

            render_submesh.index_buffer = uploader.send_buffer(
                RHIStagingBufferContext(RHIBufferUsageFlags::Index, ArrayView(submesh.indices)));
            render_submesh.index_count = submesh.indices.size();
            render_mesh.submeshes.append(render_submesh);
        }
        render_model_.append(render_mesh);
    }

    uploader.upload();

    // Unload models.
    gltf_static_meshes_unload(model_asset_path);

    // -- Uniform Buffers --
    uint32 image_count = renderer_->get_swapchain()->get_texture_views().size();
    uniform_buffers_.reserve(image_count);
    for (uint32 i = 0; i < image_count; i++) {
        RHIBufferDescription uniform_buffer_desc(
            sizeof(UniformBufferObject),
            RHIBufferUsageFlags::Uniform,
            RHIMemoryUsage::Host,
            RHISharingMode::Private);
        uniform_buffers_.append(buffer_pool_->create_buffer(uniform_buffer_desc));
    }

    // -- Shader resource pool --
    size_t total_submeshes = 0;
    for (const RenderMesh& mesh : render_model_) {
        total_submeshes += mesh.submeshes.size();
    }

    size_t total_groups_needed = image_count * (total_submeshes + 1);

    shader_resource_pool_ = device_->create_shader_resource_pool(total_groups_needed, shader_resource_binding);

    for (uint32 frame = 0; frame < image_count; frame++) {
        RHIShaderResourceGroup* group = shader_resource_pool_->allocate_group(shader_resource_group_layout_);

        RHIBuffer* uniform_buffer = uniform_buffers_[frame];
        group->set_buffer(RHIWriteBufferResource(
            ubo_binding.binding,
            uniform_buffer,
            0,
            uniform_buffer->get_size()));

        group->compile();
    }

    // Allocate SRG for every submesh * per frame.
    for (RenderMesh& mesh : render_model_) {
        for (RenderSubMesh& submesh : mesh.submeshes) {
            submesh.shader_groups.reserve(image_count);

            for (uint32 frame = 0; frame < image_count; frame++) {
                RHIShaderResourceGroup* group = shader_resource_pool_->allocate_group(shader_resource_group_layout_);

                RHIBuffer* uniform_buffer = uniform_buffers_[frame];
                group->set_buffer(RHIWriteBufferResource(
                    ubo_binding.binding,
                    uniform_buffer,
                    0,
                    uniform_buffer->get_size()));

                group->set_texture_sampler(RHIWriteTextureSamplerResource(
                    sampler_binding_.binding,
                    submesh.texture_view,
                    submesh.sampler));

                group->compile();
                submesh.shader_groups.append(group);
            }
        }
    }
}

void RenderFrameScript::on_tick(float64 delta_time) {
    if (pause_) {
        return;
    }

    update_uniform(delta_time);

    renderer_->begin_frame();
    {
        RHICommandBuffer* cmd = renderer_->get_current_command_buffer();
        float32 width = static_cast<float32>(renderer_->get_swapchain()->get_width());
        float32 height = static_cast<float32>(renderer_->get_swapchain()->get_height());

        RHIRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.render_pass = render_pass_;
        render_pass_begin_info.framebuffer = framebuffers_[renderer_->get_frame_index()];
        render_pass_begin_info.area = {.width = width, .height = height};
        render_pass_begin_info.color = Vector4f(0.01f, 0.01f, 0.01f, 1.0f);

        cmd->begin_render_pass(render_pass_begin_info);
        {
            cmd->bind_pipeline(graphics_pipeline_);

            Viewport viewport = {};
            viewport.width = width;
            viewport.height = height;
            cmd->set_viewports(&viewport, 1);

            Rect2D scissor = {};
            scissor.width = width;
            scissor.height = height;

            cmd->set_scissors(&scissor, 1);

            // Model
            {
                for (RenderMesh& mesh : render_model_) {
                    for (RenderSubMesh& submesh : mesh.submeshes) {
                        RHIShaderResourceGroup* shader_group = submesh.shader_groups[renderer_->get_current_frame()];

                        cmd->bind_vertex_buffers(submesh.vertex_buffers);
                        cmd->bind_index_buffer(submesh.index_buffer);

                        cmd->bind_shader_resource_group(graphics_pipeline_, {shader_group});

                        RHIDrawIndexedCommand draw_indexed_command = {};
                        draw_indexed_command.index_count = submesh.index_count;
                        draw_indexed_command.instance_count = 1;

                        cmd->draw(draw_indexed_command);
                    }
                }
            }
        }
        cmd->end_render_pass();
    }
    renderer_->end_frame();
}

void RenderFrameScript::update_uniform(float64 delta_time) {
    UniformBufferObject ubo;

    static float32 rotation_x = 0.0f;
    static float32 rotation_y = 0.0f;

    rotation_x += delta_time * 30.0f;
    rotation_y += delta_time * 45.0f;

    ubo.model = Matrix4f::scale(ubo.model, Vector3f(.009f));

    Quaternion fix = Quaternion::from_axis_angle(Vector3f(1.0f, 0.0f, 0.0f), Math::radians(-180.0f));
    ubo.model = ubo.model * Quaternion::rotation_matrix(fix);

    ubo.view = camera_->view;

    float32 aspect_ratio = renderer_->get_swapchain()->get_width() / static_cast<float32>(renderer_->get_swapchain()->get_height());
    ubo.proj = Matrix4f::perspective(Math::radians(75.0f), aspect_ratio, 0.1f, 10000.0f);
    ubo.proj[1][1] *= -1.0f;

    uniform_buffers_[renderer_->get_current_frame()]->update(&ubo, sizeof(UniformBufferObject), 0);
}

void RenderFrameScript::reset() {
    device_->destroy_texture_view(depth_texture_view_);
    texture_pool_->destroy_texture(depth_texture_);

    RHITextureDescription depth_texture_desc = {};
    depth_texture_desc.format = RHIFormat::D24S8;
    depth_texture_desc.usage = RHITextureUsageFlags::DepthStencilAttachment;
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
        description.render_pass = render_pass_;
        description.attachments = {texture_view, depth_texture_view_};
        description.layers = 1;

        RHIFramebuffer* framebuffer = device_->create_framebuffer(description);
        framebuffers_.append(framebuffer);
    }
}

void RenderFrameScript::on_shutdown() {
    renderer_->shutdown();

    // -- Allocators
    device_->destroy_command_allocator(cmd_allocator_);

    // -- Framebuffers --
    for (RHIFramebuffer* framebuffer : framebuffers_) {
        device_->destroy_framebuffer(framebuffer);
    }
    framebuffers_.clear();

    device_->destroy_graphics_pipeline(graphics_pipeline_);

    device_->destroy_render_pass(render_pass_);

    // -- Buffers, samplers and textures --
    device_->destroy_texture_view(depth_texture_view_);

    texture_pool_->dispose();

    buffer_pool_->dispose();

    uniform_buffers_.clear();

    // SRx
    shader_resource_pool_->dispose();
    device_->destroy_shader_resource_pool(shader_resource_pool_);

    // SwapChain
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