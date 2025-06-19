#pragma once

namespace licht {

struct VulkanContext;

void vulkan_instance_init(VulkanContext* p_context);

void vulkan_instance_destroy(VulkanContext* p_context);

}