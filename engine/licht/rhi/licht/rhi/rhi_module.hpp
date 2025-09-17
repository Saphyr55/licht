#pragma once

#include "licht/core/memory/linear_allocator.hpp"
#include "licht/core/modules/module.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/render_surface.hpp"
#include "licht/rhi/rhi_exports.hpp"

namespace licht {

enum class GraphicsAPI {
    Vulkan,
};

StringRef graphics_api_module_name(GraphicsAPI graphics_api);

class LICHT_RHI_API RHIModule : public Module {
public:
    static constexpr const char* ModuleName = "licht.rhi";

    virtual void on_load() override;

    virtual void on_startup() override;

    virtual void on_shutdown() override;

    virtual void on_unload() override;

    void on_tick();

public:
    inline void update_resized(const uint32 width, const uint32 height) {
        window_resized_ = true;
        frame_context_.frame_width = width;
        frame_context_.frame_height = height;
    }

    inline void pause() {
        pause_ = true;
    }

    inline void unpause() {
        pause_ = false;
    }

    inline void set_window_handle(WindowHandle window_handle) {
        window_handle_ = window_handle;
    }

    inline WindowHandle get_window_handle() {
        return window_handle_;
    }

    inline void set_device(RHIDeviceHandle device) {
        device_ = device;
    }

    inline GraphicsAPI get_graphics_api() {
        return graphics_api_;
    }
    
    inline void set_graphics_api(GraphicsAPI graphics_api) {
        graphics_api_ = graphics_api;
    }

private:
    void reset();

public:
    RHIModule();

private:
    using RHIFramebufferAllocator = LinearAllocator<RHIFramebufferHandle>;
    using RHIFramebufferRegistry = Array<RHIFramebufferHandle, RHIFramebufferAllocator>;
    WindowHandle window_handle_;

    GraphicsAPI graphics_api_;
    RHIRenderSurfaceHandle surface_;
    RHIDeviceHandle device_;
    RHISwapchainHandle swapchain_;
    RHIRenderPassHandle render_pass_;
    RHIPipelineHandle pipeline_;

    LinearMemoryPool framebuffer_memory_pool_;
    RHIFramebufferRegistry framebuffers_;

    RHICommandAllocatorRef command_allocator_;
    RHIFrameContext frame_context_;

    bool pause_ = false;
    bool window_resized_ = false;
};

LICHT_REGISTER_MODULE(RHIModule, RHIModule::ModuleName)

}  //namespace licht