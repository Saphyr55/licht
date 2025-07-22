#pragma once

#include "vulkan/vulkan_core.h"

namespace licht {

struct VulkanContext;

class VulkanFramebuffer {
private:
    VkFramebuffer handle_;
};

void vulkan_framebuffers_init(VulkanContext* p_context);

void vulkan_framebuffers_destroy(VulkanContext* p_context);

}  //namespace licht