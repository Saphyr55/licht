#pragma once

namespace licht {

class RHIVulkanContext;

void rhi_vulkan_instance_init(RHIVulkanContext* p_context);

void rhi_vulkan_instance_destroy(RHIVulkanContext* p_context);

}