#pragma once

namespace licht {

class RHIVulkanContext;

bool rhi_vulkan_library_load(RHIVulkanContext* p_context);

bool rhi_vulkan_core_load(RHIVulkanContext* p_context);

bool rhi_vulkan_instance_load(RHIVulkanContext* p_context);

bool rhi_vulkan_device_load(RHIVulkanContext* p_context);

}
