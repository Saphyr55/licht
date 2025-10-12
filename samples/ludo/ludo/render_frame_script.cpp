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
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/display.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/engine/project_settings.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/device_memory_uploader.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/rhi_module.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/sampler.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi/texture_view.hpp"
#include "licht/rhi/texture_pool.hpp"
#include "licht/scene/camera.hpp"

namespace licht {

RenderFrameScript::RenderFrameScript(Camera* camera)
    : window_handle_(Display::InvalidWindowHandle)
    , device_(nullptr)
    , swapchain_(nullptr)
    , render_pass_(nullptr)
    , graphics_pipeline_(nullptr)
    , framebuffers_(3)
    , graphics_command_allocator_(nullptr)
    , frame_context_()
    , pause_(false)
    , camera_(camera)
    , window_resized_(false)
    , uv_buffer_(nullptr)
    , position_buffer_(nullptr)
    , index_buffer_(nullptr)
    , shader_resource_group_layout_(nullptr) {
}

void RenderFrameScript::on_startup() {
    // RHI Module.
    ProjectSettings& project_settings = ProjectSettings::get_instance();
    ModuleRegistry& registry = ModuleRegistry::get_instance();
    RHIModule* module = registry.get_module<RHIModule>(RHIModule::ModuleName);

    // Device.
    device_ = module->get_device();

    // Window Handle.
    window_handle_ = module->get_window_handle();
    LLOG_FATAL_WHEN(
        !Display::get_default().is_valid(window_handle_),
        "[RHIModule]",
        "Failed to retrieve a valid window handle. Ensure a window is created before initializing the RHI Module.");

    // Swapchain.
    WindowStatues window_statues = Display::get_default().query_window_statues(window_handle_);
    frame_context_.frame_height = static_cast<uint32>(window_statues.height);
    frame_context_.frame_width = static_cast<uint32>(window_statues.width);
    swapchain_ = device_->create_swapchain(frame_context_.frame_width, frame_context_.frame_height, frame_context_.frame_count);

    // Queues.
    const Array<RHICommandQueueRef>& command_queues = device_->get_command_queues();

    RHICommandQueueRef* graphics_command_queue_ptr = command_queues.get_if([](RHICommandQueueRef command_queue) {
        return command_queue->is_graphics_type();
    });
    LCHECK_MSG(graphics_command_queue_ptr, "Found no graphics command queue.");
    graphics_command_queue_ = *graphics_command_queue_ptr;

    RHICommandQueueRef* graphics_present_command_queue_ptr = command_queues.get_if([](RHICommandQueueRef command_queue) {
        return command_queue->is_graphics_type() && command_queue->is_present_mode();
    });
    LCHECK_MSG(graphics_present_command_queue_ptr, "Found no graphics command queue that support the present mode.");
    graphics_present_command_queue_ = *graphics_present_command_queue_ptr;

    // Pools
    buffer_pool_ = device_->create_buffer_pool();
    buffer_pool_->initialize_pool(&DefaultAllocator::get_instance(), 64);

    texture_pool_ = device_->create_texture_pool();
    texture_pool_->initialize_pool(&DefaultAllocator::get_instance(), 64);

    // Command Allocators
    RHICommandAllocatorDescription graphics_command_allocator_desc = {};
    graphics_command_allocator_desc.count = frame_context_.frame_count;
    graphics_command_allocator_desc.command_queue = graphics_command_queue_;

    graphics_command_allocator_ = device_->create_command_allocator(graphics_command_allocator_desc);

    // Render Pass.
    RHIColorAttachmentDescription render_pass_color_attachment = {};
    render_pass_color_attachment.format = swapchain_->get_format();

    RHIRenderPassDescription render_pass_description = {};
    render_pass_description.attachment_decriptions = {render_pass_color_attachment};

    render_pass_ = device_->create_render_pass(render_pass_description);

    RHIShaderResourceBinding ubo_binding(
        0, RHIShaderResourceType::Uniform, RHIShaderStage::Fragment | RHIShaderStage::Vertex, 1);

    RHIShaderResourceBinding orange_sampler_binding(
        1, RHIShaderResourceType::Sampler, RHIShaderStage::Fragment);

    Array<RHIShaderResourceBinding> shader_resource_binding = {ubo_binding, orange_sampler_binding};
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

    // Graphics Pipeline.
    RHIGraphicsPipelineVertexBindingInformation pipeline_vertex_binding_information = {
        .bindings = {position_input_binding_description, texture_uv_input_binding_description},
        .attributes = {position_attribute_description, texture_uv_attribute_description}};

    float32 width = swapchain_->get_width();
    float32 height = swapchain_->get_height();

    // Load shaders binary codes.
    FileSystem& file_system = FileSystem::get_platform();

    FileHandleResult vertex_file_open_error = file_system.open_read("shaders/main.vert.spv");
    LCHECK(vertex_file_open_error.has_value());

    SharedRef<FileHandle> vertex_file_handle = vertex_file_open_error.value();
    SPIRVShader vertex_shader(vertex_file_handle->read_all_bytes());

    FileHandleResult fragment_file_open_error = file_system.open_read("shaders/main.frag.spv");
    LCHECK(fragment_file_open_error.has_value());

    SharedRef<FileHandle> fragment_file_handle = fragment_file_open_error.value();
    SPIRVShader fragment_shader(fragment_file_handle->read_all_bytes());

    RHIGraphicsPipelineViewportStateInformation viewport_info = {
        .viewport = Viewport{
            .width = width,
            .height = height,
        },
        .scissor = Rect2D{
            .width = width,
            .height = height,
        },
    };

    // -- Graphics Pipeline Shaders --
    RHIGraphicsPipelineShaderStageCreateInfo vertex_stage_create_info = {
        .name = "main",
        .shader = vertex_shader,
        .type = RHIShaderStage::Vertex,
    };

    RHIGraphicsPipelineShaderStageCreateInfo fragment_stage_create_info = {
        .name = "main",
        .shader = fragment_shader,
        .type = RHIShaderStage::Fragment,
    };

    // -- Graphics Pipeline --
    graphics_pipeline_ = device_->create_graphics_pipeline(
        RHIGraphicsPipeline::description_builder()
            .with_render_pass(render_pass_)
            .with_vertex_shader(vertex_stage_create_info.shader, vertex_stage_create_info.name)
            .with_fragment_shader(fragment_stage_create_info.shader, fragment_stage_create_info.name)
            .with_vertex_bindings(pipeline_vertex_binding_information.bindings, pipeline_vertex_binding_information.attributes)
            .with_viewport(viewport_info.viewport, viewport_info.scissor)
            .with_shader_resource_group_layout(shader_resource_group_layout_)
            .with_cull_mode(RHICullModeFlags::Back)
            .build());

    depth_texture_ = texture_pool_->create_texture(RHITextureDescription{
        .format = RHIFormat::D24S8,
        .usage =  RHITextureUsageFlags::DepthStencilAttachment,
        .width =  width,
        .height = height,
    });

    depth_texture_view_ = device_->create_texture_view(RHITextureViewDescription{
        .texture = depth_texture_,
        .format = depth_texture_->get_description().format,
    });

    // -- Framebuffers --
    framebuffers_.reserve(swapchain_->get_texture_views().size());
    for (RHITextureView* texture : swapchain_->get_texture_views()) {
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

    // Load images.
    String projectdir(project_settings.get_name("projectdir"));
    String orange_asset_path = projectdir;
    orange_asset_path.append("/assets/prototype_textures/orange_1.png");

    Image* orange_image = Image::load("orange_1", orange_asset_path);
    RHITextureDescription orange_texture_description;
    orange_texture_description.format = RHIFormat::RGBA8sRGB;
    orange_texture_description.memory_usage = RHIMemoryUsage::Device;
    orange_texture_description.usage = RHITextureUsageFlags::Sampled;
    orange_texture_description.sharing_mode = RHISharingMode::Shared;
    orange_texture_description.width = orange_image->get_width();
    orange_texture_description.height = orange_image->get_heigth();

    // Device objects --
    RHIDeviceMemoryUploader uploader(device_, buffer_pool_, texture_pool_);

    // Vertex
    position_buffer_ = uploader.send_buffer(RHIStagingBufferContext(
        RHIBufferUsageFlags::Vertex, ArrayView(cube_mesh_.positions)));

    uv_buffer_ = uploader.send_buffer(RHIStagingBufferContext(
        RHIBufferUsageFlags::Vertex, ArrayView(cube_mesh_.UVs)));

    index_buffer_ = uploader.send_buffer(RHIStagingBufferContext(
        RHIBufferUsageFlags::Index, ArrayView(cube_mesh_.indices)));

    // Texture
    orange_texture_ = uploader.send_texture(RHIStagingBufferContext(
                                                RHIBufferUsageFlags::Storage,
                                                orange_image->get_size(),
                                                orange_image->data()),
                                            orange_texture_description);

    uploader.upload();

    // Unload images.
    Image::unload(orange_image);

    // Texture views.
    orange_texture_view_ = device_->create_texture_view(RHITextureViewDescription{
        .texture = orange_texture_,
        .format = orange_texture_->get_description().format,
        .dimension = RHITextureDimension::Dim2D,
    });

    // Samplers
    orange_texture_sampler_ = device_->create_sampler(RHISamplerDescription{});

    // -- Uniform Buffers --
    uint32 image_count = swapchain_->get_texture_views().size();
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
    shader_resource_pool_ = device_->create_shader_resource_pool(image_count, shader_resource_binding);
    for (uint32 i = 0; i < image_count; i++) {
        RHIShaderResourceGroup* shader_resource_group = shader_resource_pool_->allocate_group(shader_resource_group_layout_);

        RHIBuffer* uniform_buffer = uniform_buffers_[i];

        shader_resource_group->set_buffer(
            RHIWriteBufferResource(
                ubo_binding.binding,
                uniform_buffer,
                0,
                uniform_buffer->get_size()));

        shader_resource_group->set_texture_sampler(
            RHIWriteTextureSamplerResource(
                orange_sampler_binding.binding,
                orange_texture_view_,
                orange_texture_sampler_));

        shader_resource_group->compile();
    }

    // -- Frame Context Sync --
    frame_context_.frame_available_semaphores.reserve(frame_context_.frame_count);
    frame_context_.render_finished_semaphores.reserve(frame_context_.frame_count);
    frame_context_.in_flight_fences.reserve(frame_context_.frame_count);
    frame_context_.frame_in_flight_fences.reserve(swapchain_->get_texture_views().size());

    for (uint32 i = 0; i < frame_context_.frame_count; i++) {
        frame_context_.frame_available_semaphores.append(device_->create_semaphore());
        frame_context_.render_finished_semaphores.append(device_->create_semaphore());
        frame_context_.in_flight_fences.append(device_->create_fence());
    }

    for (uint32 i = 0; i < swapchain_->get_texture_views().size(); i++) {
        frame_context_.frame_in_flight_fences.append(nullptr);
    }
}

void RenderFrameScript::on_tick(float64 delta_time) {
    if (pause_) {
        return;
    }

    swapchain_->acquire_next_frame(frame_context_);

    if (frame_context_.out_of_date) {
        reset();
        return;
    }
    LCHECK(frame_context_.success);

    update_uniform(delta_time);

    RHICommandBuffer* graphics_command_buffer = graphics_command_allocator_->open(frame_context_.current_frame);
    graphics_command_allocator_->reset_command_buffer(graphics_command_buffer);

    graphics_command_buffer->begin();
    {
        float32 width = static_cast<float32>(swapchain_->get_width());
        float32 height = static_cast<float32>(swapchain_->get_height());

        Rect2D render_pass_area = {};
        render_pass_area.x = 0.0f;
        render_pass_area.y = 0.0f;
        render_pass_area.width = width;
        render_pass_area.height = height;

        RHIRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.render_pass = render_pass_;
        render_pass_begin_info.framebuffer = framebuffers_[frame_context_.frame_index];
        render_pass_begin_info.area = render_pass_area;
        render_pass_begin_info.color = Vector4f(0.01f, 0.01f, 0.01f, 1.0f);

        graphics_command_buffer->begin_render_pass(render_pass_begin_info);
        {
            graphics_command_buffer->bind_pipeline(graphics_pipeline_);

            Viewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = width;
            viewport.height = height;
            viewport.min_depth = 0.0f;
            viewport.max_depth = 1.0f;

            graphics_command_buffer->set_viewports(&viewport, 1);

            Rect2D scissor = {};
            scissor.x = 0.0f;
            scissor.y = 0.0f;
            scissor.width = width;
            scissor.height = height;

            graphics_command_buffer->set_scissors(&scissor, 1);

            graphics_command_buffer->bind_vertex_buffers({position_buffer_, uv_buffer_});
            graphics_command_buffer->bind_index_buffer(index_buffer_);

            RHIShaderResourceGroup* shader_resource_group = shader_resource_pool_->get_group(frame_context_.current_frame);
            graphics_command_buffer->bind_shader_resource_group(graphics_pipeline_, {shader_resource_group});

            RHIDrawIndexedCommand draw_indexed_command = {};
            draw_indexed_command.index_count = cube_mesh_.indices.size();
            draw_indexed_command.instance_count = 1;

            graphics_command_buffer->draw(draw_indexed_command);
        }
        graphics_command_buffer->end_render_pass();
    }
    graphics_command_buffer->end();

    frame_context_.frame_in_flight_fences[frame_context_.frame_index] =
        &frame_context_.in_flight_fences[frame_context_.current_frame];

    device_->reset_fence(frame_context_.in_flight_fences[frame_context_.current_frame]);

    graphics_command_queue_->submit({graphics_command_buffer},
                                    {frame_context_.current_frame_available_semaphore()},
                                    {frame_context_.current_render_finished_semaphore()},
                                    frame_context_.current_in_flight_fence());

    graphics_present_command_queue_->present(swapchain_, frame_context_);

    if (window_resized_ || frame_context_.suboptimal || frame_context_.out_of_date) {
        reset();
        window_resized_ = false;
    }

    LCHECK(frame_context_.success);
    device_->wait_fence(frame_context_.in_flight_fences[frame_context_.current_frame]);

    frame_context_.next_frame();
}

void RenderFrameScript::update_uniform(float64 delta_time) {
    UniformBufferObject ubo;

    static float32 rotation_x = 0.0f;
    static float32 rotation_y = 0.0f;

    rotation_x += delta_time * 30.0f;
    rotation_y += delta_time * 45.0f;

    Quaternion rot_x = Quaternion::from_axis_angle(Vector3f(1.0f, 0.0f, 0.0f), Math::radians(rotation_x));
    Quaternion rot_y = Quaternion::from_axis_angle(Vector3f(0.0f, 1.0f, 0.0f), Math::radians(rotation_y));
    Quaternion rotation = rot_x * rot_y;

    ubo.model = Quaternion::rotation_matrix(rotation);

    ubo.view = camera_->view;

    float32 aspect_ratio = swapchain_->get_width() / static_cast<float32>(swapchain_->get_height());
    ubo.proj = Matrix4f::perspective(Math::radians(75.0f), aspect_ratio, 0.00001f, 10000.0f);
    ubo.proj[1][1] *= -1.0f;

    uniform_buffers_[frame_context_.current_frame]->update(&ubo, sizeof(UniformBufferObject), 0);
}

void RenderFrameScript::reset() {
    device_->wait_idle();

    device_->destroy_texture_view(depth_texture_view_);
    texture_pool_->destroy_texture(depth_texture_);

    RHITextureDescription depth_texture_desc = {};
    depth_texture_desc.format = RHIFormat::D24S8;
    depth_texture_desc.usage = RHITextureUsageFlags::DepthStencilAttachment;
    depth_texture_desc.width = frame_context_.frame_width;
    depth_texture_desc.height = frame_context_.frame_height;
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

    // Recreate swapchain
    device_->recreate_swapchain(swapchain_, frame_context_.frame_width, frame_context_.frame_height);

    // Create new framebuffers
    framebuffers_.reserve(swapchain_->get_texture_views().size());
    for (RHITextureView* texture_view : swapchain_->get_texture_views()) {
        RHIFramebufferDescription description = {};
        description.height = swapchain_->get_height();
        description.width = swapchain_->get_width();
        description.render_pass = render_pass_;
        description.attachments = {texture_view, depth_texture_view_};
        description.layers = 1;

        RHIFramebuffer* framebuffer = device_->create_framebuffer(description);
        framebuffers_.append(framebuffer);
    }
}

void RenderFrameScript::on_shutdown() {
    device_->wait_idle();

    // -- Frame Context --
    for (uint32 i = 0; i < frame_context_.frame_count; i++) {
        device_->destroy_semaphore(frame_context_.frame_available_semaphores[i]);
        device_->destroy_semaphore(frame_context_.render_finished_semaphores[i]);
        device_->destroy_fence(frame_context_.in_flight_fences[i]);
    }
    frame_context_.frame_available_semaphores.clear();
    frame_context_.render_finished_semaphores.clear();
    frame_context_.in_flight_fences.clear();
    frame_context_.frame_in_flight_fences.clear();

    // -- Allocators
    device_->destroy_command_allocator(graphics_command_allocator_);

    // -- Framebuffers --
    for (RHIFramebuffer* framebuffer : framebuffers_) {
        device_->destroy_framebuffer(framebuffer);
    }
    framebuffers_.clear();
    framebuffer_memory_allocator_.reset();

    device_->destroy_graphics_pipeline(graphics_pipeline_);

    device_->destroy_render_pass(render_pass_);

    // -- Buffers, samplers and textures --
    device_->destroy_sampler(orange_texture_sampler_);

    device_->destroy_texture_view(orange_texture_view_);
    device_->destroy_texture_view(depth_texture_view_);

    texture_pool_->dispose();

    buffer_pool_->dispose();

    uniform_buffers_.clear();

    // SRx
    for (size_t i = 0; i < swapchain_->get_texture_views().size(); i++) {
        RHIShaderResourceGroup* group = shader_resource_pool_->get_group(i);
        shader_resource_pool_->deallocate_group(group);
    }
    device_->destroy_shader_resource_pool(shader_resource_pool_);

    // SwapChain
    device_->destroy_swapchain(swapchain_);
}

void RenderFrameScript::update_resized(const uint32 width, const uint32 height) {
    window_resized_ = true;
    frame_context_.frame_width = width;
    frame_context_.frame_height = height;
}

void RenderFrameScript::pause() {
    pause_ = true;
}

void RenderFrameScript::unpause() {
    pause_ = false;
}

}  // namespace licht