#pragma once

namespace licht {

struct VulkanContext;

bool vulkan_library_load(VulkanContext* p_context);

bool vulkan_core_load(VulkanContext* p_context);

bool vulkan_instance_load(VulkanContext* p_context);

bool vulkan_device_load(VulkanContext* p_context);

}
