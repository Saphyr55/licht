#pragma once

namespace licht {

struct VulkanContext;

void vulkan_debug_messenger_init(VulkanContext* p_context);

void vulkan_debug_messenger_destroy(VulkanContext* p_context);

}