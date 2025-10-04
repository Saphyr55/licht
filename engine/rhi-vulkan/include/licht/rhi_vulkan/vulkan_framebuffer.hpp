#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanFramebuffer : public RHIFramebuffer {
public:
    VkFramebuffer& get_handle();

    void initialize();

    void destroy();

public:
    RHIVulkanFramebuffer(VulkanContext& context, const RHIFramebufferDescription& description);
    virtual ~RHIVulkanFramebuffer() override;

private:
    RHIFramebufferDescription description_;
    VkFramebuffer handle_;
    VulkanContext& context_;
};

using RHIVulkanFramebufferRef = SharedRef<RHIVulkanFramebuffer>;


}  //namespace licht