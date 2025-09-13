#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/render_surface.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

struct VulkanContext;

class RHIVulkanRenderSurface : public RHIRenderSurface {
public:
    static SharedRef<RHIVulkanRenderSurface> create(VulkanContext& context, void* window_handle);

    virtual void* get_platform_window() override {
        return window_handle_;
    }

    virtual void initialize() = 0;

    virtual void destroy() = 0;

    VkSurfaceKHR& get_handle() {
        return handle_;
    }

protected:
    RHIVulkanRenderSurface(VkInstance instance, VkAllocationCallbacks* allocator, void* window_handle) 
        : instance_(instance)
        , allocator_(allocator)
        , window_handle_(window_handle)
        , handle_(VK_NULL_HANDLE) {
    }

protected:
    VkInstance instance_;
    VkSurfaceKHR handle_;
    VkAllocationCallbacks* allocator_;
    void* window_handle_;
};

using RHIVulkanRenderSurfaceRef = SharedRef<RHIVulkanRenderSurface>;

}