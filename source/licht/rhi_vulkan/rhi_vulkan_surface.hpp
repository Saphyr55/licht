#pragma once

namespace licht {

class RHIVulkanContext;

void rhi_vulkan_surface_init(RHIVulkanContext* p_context, void* p_window_handle);

void rhi_vulkan_surface_destroy(RHIVulkanContext* p_context);

}