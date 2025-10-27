#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/display.hpp"
#include "licht/renderer/render_context.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/device_memory_uploader.hpp"
#include "licht/rhi/rhi_module.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/buffer_pool.hpp"
#include "licht/rhi/texture_pool.hpp"

namespace licht {

void RenderContext::initialize(WindowHandle window_handle) {

    ModuleRegistry& registry = ModuleRegistry::get_instance();
    RHIModule* module = registry.get_module<RHIModule>(RHIModule::ModuleName);
    
    device_ = module->get_device();

    set_window_handle(window_handle);
    set_graphics_queue(device_->get_graphics_queue());
    set_present_queue(device_->get_present_queue());

    buffer_pool_ = device_->create_buffer_pool();
    buffer_pool_->initialize_pool(&DefaultAllocator::get_instance(), 64);

    texture_pool_ = device_->create_texture_pool();
    texture_pool_->initialize_pool(&DefaultAllocator::get_instance(), 64);
    
    command_allocator_ = device_->create_command_allocator({
        .command_queue = graphics_queue_,
        .count = get_frame_count(),
    });
    
    WindowStatues window_statues = Display::get_default().query_window_statues(window_handle_);
    frame_context_.frame_height = static_cast<uint32>(window_statues.height);
    frame_context_.frame_width = static_cast<uint32>(window_statues.width);
    swapchain_ = device_->create_swapchain(
        frame_context_.frame_width,
        frame_context_.frame_height,
        frame_context_.frame_count);

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

void RenderContext::shutdown() {
    device_->wait_idle();
    device_->destroy_command_allocator(command_allocator_);

    buffer_pool_->dispose();
    texture_pool_->dispose();

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

RenderResult RenderContext::begin_frame() {
    swapchain_->acquire_next_frame(frame_context_);

    if (frame_context_.out_of_date) {
        reset();
        return RenderResult::OutOfDate;
    }

    current_cmd_ = command_allocator_->open(frame_context_.current_frame);
    command_allocator_->reset_command_buffer(current_cmd_);

    current_cmd_->begin();

    return RenderResult::Success;
}

RenderResult RenderContext::end_frame() {
    current_cmd_->end();

    frame_context_.frame_in_flight_fences[frame_context_.frame_index] =
        &frame_context_.in_flight_fences[frame_context_.current_frame];

    device_->reset_fence(frame_context_.in_flight_fences[frame_context_.current_frame]);

    graphics_queue_->submit({current_cmd_},
                            {frame_context_.current_frame_available_semaphore()},
                            {frame_context_.current_render_finished_semaphore()},
                            frame_context_.current_in_flight_fence());

    present_queue_->present(swapchain_, frame_context_);

    if (window_resized_ || frame_context_.suboptimal || frame_context_.out_of_date) {
        reset();
        window_resized_ = false;
    }

    if (!frame_context_.success) {
        return RenderResult::Unknown;
    }
    device_->wait_fence(frame_context_.in_flight_fences[frame_context_.current_frame]);

    frame_context_.next_frame();

    return RenderResult::Success;
}

void RenderContext::reset() {
    device_->wait_idle();
    device_->recreate_swapchain(swapchain_, frame_context_.frame_width, frame_context_.frame_height);

    on_reset_();
}

RHISwapchain* RenderContext::get_swapchain() {
    return swapchain_;
}

RHIDeviceRef RenderContext::get_device() {
    return device_;
}

}  //namespace licht
