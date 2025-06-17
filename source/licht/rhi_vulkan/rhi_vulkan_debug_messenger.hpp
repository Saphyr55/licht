#pragma once

namespace licht {

class RHIVulkanContext;

void rhi_vulkan_debug_messenger_init(RHIVulkanContext* p_context);

void rhi_vulkan_debug_messenger_destroy(RHIVulkanContext* p_context);

}