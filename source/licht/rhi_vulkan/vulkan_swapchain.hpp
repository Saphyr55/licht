#pragma once

namespace licht {

struct VulkanContext;

void vulkan_swapchain_init(VulkanContext* p_context);

void vulkan_swapchain_recreate(VulkanContext* p_context);

void vulkan_swapchain_destroy(VulkanContext* p_context);

}

