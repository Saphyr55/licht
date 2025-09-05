#pragma once

#include "licht/core/defines.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

struct VulkanContext;

class VulkanFramebuffer {
public:
    VulkanFramebuffer();
    
private:
    VkImageView image_view_;
    VkRenderPass render_pass_;
    VkFramebuffer handle_;
};

void vulkan_framebuffers_init(VulkanContext* p_context);

void vulkan_framebuffers_destroy(VulkanContext* p_context);

}  //namespace licht