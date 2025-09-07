#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanFramebuffer : public RHIFramebuffer {
public:
    VkFramebuffer& get_handle();

public:
    virtual ~RHIVulkanFramebuffer() override = default;

private:
    VkFramebuffer handle_;
};

using RHIVulkanFramebufferRef = SharedRef<RHIVulkanFramebuffer>;

void vulkan_framebuffer_init(VulkanContext& context, RHIVulkanFramebufferRef framebuffer, const RHIFramebufferDescription& description);

void vulkan_framebuffer_destroy(VulkanContext& context, RHIVulkanFramebufferRef framebuffer);


}  //namespace licht