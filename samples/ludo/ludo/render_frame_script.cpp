#include "render_frame_script.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/io/file_handle.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/display.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/rhi/rhi_module.hpp"

namespace licht {

RenderFrameScript::RenderFrameScript()
    : window_handle_(Display::InvalidWindowHandle)
    , device_(nullptr)
    , swapchain_(nullptr)
    , render_pass_(nullptr)
    , pipeline_(nullptr)
    , framebuffer_memory_allocator_(1024 /* 1 kB */)
    , framebuffers_(4, RHIFramebufferAllocator(framebuffer_memory_allocator_))
    , command_allocator_(nullptr)
    , frame_context_()
    , pause_(false)
    , window_resized_(false) {
}

RenderFrameScript::~RenderFrameScript() {}

void RenderFrameScript::on_startup() {
    ModuleRegistry& registry = ModuleRegistry::get_instance();
    RHIModule* module = registry.get_module<RHIModule>(RHIModule::ModuleName);

    // -- Device --
    {
        device_ = module->get_device();
        LCHECK(device_);
    }

    // -- Window Handle --
    {
        window_handle_ = module->get_window_handle();
        LLOG_FATAL_WHEN(!Display::get_default().is_valid(window_handle_), "[RHIModule]",
                        "Failed to retrieve a valid window handle. Ensure a window is created before initializing the RHI Module.");
    }

    // -- Swapchain --
    {
        WindowStatues window_statues = Display::get_default().query_window_statues(window_handle_);
        frame_context_.frame_height = static_cast<uint32>(window_statues.height);
        frame_context_.frame_width = static_cast<uint32>(window_statues.width);
        swapchain_ = device_->create_swapchain(frame_context_.frame_width, frame_context_.frame_height);
    }

    // -- Render Pass --
    {
        RHIAttachmentDescription render_pass_color_attachment = {};
        render_pass_color_attachment.format = swapchain_->get_format();

        RHIRenderPassDescription render_pass_description = {};
        render_pass_description.attachment_decriptions = {render_pass_color_attachment};

        render_pass_ = device_->create_render_pass(render_pass_description);
    }

    // -- Graphics Pipeline --
    {
        float32 width = static_cast<float32>(swapchain_->get_width());
        float32 height = static_cast<float32>(swapchain_->get_height());

        FileSystem& file_system = FileSystem::get_platform();

        FileHandleResult vertex_file_open_error = file_system.open_read("shaders/main.vert.spv");
        LCHECK(vertex_file_open_error.has_value())
        SharedRef<FileHandle> vertex_file_handle = vertex_file_open_error.value();
        CompiledShader vertex_shader(vertex_file_handle->read_all_bytes());

        FileHandleResult fragment_file_open_error = file_system.open_read("shaders/main.frag.spv");
        LCHECK(fragment_file_open_error.has_value())
        SharedRef<FileHandle> fragment_file_handle = fragment_file_open_error.value();
        CompiledShader fragment_shader(fragment_file_handle->read_all_bytes());

        // -- Graphics Pipeline Shaders --
        RHIPipelineShaderStageCreateInfo vertex_stage_create_info;
        vertex_stage_create_info.name = "main";
        vertex_stage_create_info.shader = &vertex_shader;
        vertex_stage_create_info.type = ShaderStageType::Vertex;

        RHIPipelineShaderStageCreateInfo fragment_stage_create_info;
        fragment_stage_create_info.name = "main";
        fragment_stage_create_info.shader = &fragment_shader;
        fragment_stage_create_info.type = ShaderStageType::Fragment;

        // -- Graphics Pipeline --
        Viewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = width;
        viewport.height = height;
        viewport.min_depth = 0.0f;
        viewport.max_depth = 1.0f;

        Rect2D scissor = {};
        scissor.x = 0.0f;
        scissor.y = 0.0f;
        scissor.width = width;
        scissor.height = height;

        RHIPipelineViewportStateInformation viewport_info;
        viewport_info.viewport = viewport;
        viewport_info.scissor = scissor;

        RHIPipelineDescription pipeline_description;
        pipeline_description.render_pass = render_pass_;
        pipeline_description.vertex_shader_info = vertex_stage_create_info;
        pipeline_description.fragment_shader_info = fragment_stage_create_info;
        pipeline_description.viewport_info = viewport_info;
        pipeline_ = device_->create_graphics_pipeline(pipeline_description);
    }

    // -- Framebuffers --
    {
        framebuffers_.reserve(swapchain_->get_texture_views().size());
        for (RHITextureViewHandle texture : swapchain_->get_texture_views()) {
            RHIFramebufferDescription description = {};
            description.height = swapchain_->get_height();
            description.width = swapchain_->get_width();
            description.render_pass = render_pass_;
            description.attachments = {texture};
            description.layers = 1;

            RHIFramebufferHandle framebuffer = device_->create_framebuffer(render_pass_, description);
            framebuffers_.append(framebuffer);
        }
    }

    // -- Command Allocator
    {
        command_allocator_ = device_->create_command_allocator(frame_context_.frame_count);
    }

    // -- Frame Context Sync --
    {
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
}

void RenderFrameScript::on_tick(float32 delta_time) {
    if (pause_) {
        return;
    }

    swapchain_->acquire_next_frame(frame_context_);

    if (frame_context_.out_of_date) {
        reset();
        return;
    }
    LCHECK(frame_context_.success);

    RHICommandBufferHandle command_buffer = command_allocator_->open(frame_context_.current_frame);
    command_allocator_->reset_command_buffer(command_buffer);

    float32 width = static_cast<float32>(swapchain_->get_width());
    float32 height = static_cast<float32>(swapchain_->get_height());

    command_buffer->begin();
    {
        Rect2D render_pass_area = {};
        render_pass_area.x = 0.0f;
        render_pass_area.y = 0.0f;
        render_pass_area.width = width;
        render_pass_area.height = height;

        RHIRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.render_pass = render_pass_;
        render_pass_begin_info.framebuffer = framebuffers_[frame_context_.frame_index];
        render_pass_begin_info.area = render_pass_area;
        command_buffer->begin_render_pass(render_pass_begin_info);
        {
            command_buffer->bind_pipeline(pipeline_);

            Viewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = width;
            viewport.height = height;
            viewport.min_depth = 0.0f;
            viewport.max_depth = 1.0f;
            command_buffer->set_viewports(&viewport, 1);

            Rect2D scissor = {};
            scissor.x = 0.0f;
            scissor.y = 0.0f;
            scissor.width = width;
            scissor.height = height;
            command_buffer->set_scissors(&scissor, 1);

            RHIDrawCommand draw_command = {};
            draw_command.vertex_count = 3;
            draw_command.instance_count = 1;
            draw_command.first_instance = 0;
            draw_command.first_vertex = 0;
            command_buffer->draw(draw_command);
        }
        command_buffer->end_render_pass();
    }
    command_buffer->end();

    if (RHIFenceHandle* fence = frame_context_.frame_in_flight_fences[frame_context_.frame_index]) {
        device_->wait_fence(*fence);
    }

    frame_context_.frame_in_flight_fences[frame_context_.frame_index] = &frame_context_.in_flight_fences[frame_context_.current_frame];

    device_->reset_fence(frame_context_.in_flight_fences[frame_context_.current_frame]);

    RHICommandQueueRef graphics_queue = device_->query_queue(RHIQueueType::Graphics);
    RHICommandQueueRef present_queue = device_->query_queue(RHIQueueType::Transfer);

    graphics_queue->submit({command_buffer}, frame_context_);
    present_queue->present(swapchain_, frame_context_);

    if (window_resized_ || frame_context_.suboptimal || frame_context_.out_of_date) {
        reset();
        window_resized_ = false;
    }

    LCHECK(frame_context_.success);
    device_->wait_fence(frame_context_.in_flight_fences[frame_context_.current_frame]);

    frame_context_.next_frame();
}

void RenderFrameScript::reset() {
    device_->wait_idle();

    for (RHIFramebufferHandle framebuffer : framebuffers_) {
        device_->destroy_framebuffer(framebuffer);
    }
    framebuffers_.clear();
    framebuffer_memory_allocator_.reset();

    device_->recreate_swapchain(swapchain_, frame_context_.frame_width, frame_context_.frame_height);

    framebuffers_.reserve(swapchain_->get_texture_views().size());
    for (RHITextureViewHandle texture : swapchain_->get_texture_views()) {
        RHIFramebufferDescription description = {};
        description.height = swapchain_->get_height();
        description.width = swapchain_->get_width();
        description.render_pass = render_pass_;
        description.attachments = {texture};
        description.layers = 1;

        RHIFramebufferHandle framebuffer = device_->create_framebuffer(render_pass_, description);
        framebuffers_.append(framebuffer);
    }
}

void RenderFrameScript::on_shutdown() {
    device_->wait_idle();

    // -- Frame Context Sync --
    {
        for (uint32 i = 0; i < frame_context_.frame_count; i++) {
            device_->destroy_semaphore(frame_context_.frame_available_semaphores[i]);
            device_->destroy_semaphore(frame_context_.render_finished_semaphores[i]);
            device_->destroy_fence(frame_context_.in_flight_fences[i]);
        }
        frame_context_.frame_available_semaphores.clear();
        frame_context_.render_finished_semaphores.clear();
        frame_context_.in_flight_fences.clear();
        frame_context_.frame_in_flight_fences.clear();
    }

    // -- Allocator
    {
        device_->destroy_command_allocator(command_allocator_);
    }

    // -- Framebuffers --
    {
        for (RHIFramebufferHandle framebuffer : framebuffers_) {
            device_->destroy_framebuffer(framebuffer);
        }
        framebuffers_.clear();
    }

    device_->destroy_graphics_pipeline(pipeline_);

    device_->destroy_render_pass(render_pass_);

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