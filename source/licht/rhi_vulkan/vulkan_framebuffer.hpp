#pragma once

namespace licht {

struct VulkanContext;

void vulkan_framebuffers_init(VulkanContext* p_context);

void vulkan_framebuffers_destroy(VulkanContext* p_context);

}