#pragma once

namespace licht {

struct VulkanContext;

void vulkan_surface_init(VulkanContext* p_context, void* p_window_handle);

void vulkan_surface_destroy(VulkanContext* p_context);

}