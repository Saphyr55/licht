#pragma once

namespace licht {

struct VulkanContext;

void vulkan_command_pool_init(VulkanContext* p_context);

void vulkan_command_pool_destroy(VulkanContext* p_context);

}