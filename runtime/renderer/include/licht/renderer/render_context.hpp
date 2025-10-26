#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/platform/display.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/renderer/renderer_exports.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/swapchain.hpp"

namespace licht {

enum class RenderResult {
    Success = 0,
    OutOfDate,
    Unknown,
};

class LICHT_RENDERER_API RenderContext {
public:
    void initialize(WindowHandle window_handle,
                    const RHICommandQueueRef& graphics_queue,
                    const RHICommandQueueRef& present_queue,
                    RHICommandAllocator* cmd_allocator);

    void shutdown();

    RenderResult begin_frame();

    RenderResult end_frame();

    RHISwapchain* get_swapchain();

    RHIDeviceRef get_device();

    WindowHandle get_window_handle() {
        return window_handle_;
    }

    const RHIFrameContext& get_frame_context() const {
        return frame_context_;
    }


    RHICommandAllocator* get_command_allocator() {
        return command_allocator_;
    }

    const RHICommandAllocator* get_command_allocator() const {
        return command_allocator_;
    }

    void set_command_allocator(RHICommandAllocator* cmd_allocator) {
        command_allocator_ = cmd_allocator;
    }

    void update_resized(const uint32 width, const uint32 height) {
        window_resized_ = true;
        frame_context_.frame_width = width;
        frame_context_.frame_height = height;
    }

    void on_reset(Function<void()>&& on_reset) {
        on_reset_ = on_reset;
    }

    uint32 get_frame_count() const {
        return frame_context_.frame_count;
    }

    uint32 get_current_frame() const {
        return frame_context_.current_frame;
    }

    uint32 get_frame_index() const {
        return frame_context_.frame_index;
    }

    RHICommandQueueRef get_present_queue() {
        return present_queue_;
    }

    RHICommandQueueRef get_graphics_queue() {
        return graphics_queue_;
    }

    const RHICommandQueueRef& get_present_queue() const {
        return present_queue_;
    }

    const RHICommandQueueRef& get_graphics_queue() const {
        return graphics_queue_;
    }

    RHICommandBuffer* get_current_command_buffer() {
        return current_cmd_;
    }

    const RHICommandBuffer* get_current_command_buffer() const {
        return current_cmd_;
    }

private:
    void reset();

    void set_graphics_queue(const RHICommandQueueRef& graphics_queue) {
        graphics_queue_ = graphics_queue;
    }

    void set_present_queue(const RHICommandQueueRef& present_queue) {
        present_queue_ = present_queue;
    }

    void set_window_handle(WindowHandle window_handle) {
        window_handle_ = window_handle;
    }

public:
    RenderContext()
        : window_handle_(Display::InvalidWindowHandle) {}

private:
    Function<void()> on_reset_;
    WindowHandle window_handle_;
    RHIFrameContext frame_context_;
    RHIDeviceRef device_;
    RHICommandAllocator* command_allocator_;
    RHISwapchain* swapchain_;
    RHICommandBuffer* current_cmd_;
    RHICommandQueueRef present_queue_;
    RHICommandQueueRef graphics_queue_;
    bool window_resized_ = false;
};

}  //namespace licht