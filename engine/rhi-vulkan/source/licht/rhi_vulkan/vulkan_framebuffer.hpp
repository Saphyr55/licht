#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class VulkanFramebuffer : public RHIFramebuffer {
public:
    VkFramebuffer& get_handle();

    const VkFramebuffer& get_handle() const;

    void initialize();

    void destroy();

public:
    VulkanFramebuffer(VulkanContext& context, const RHIFramebufferDescription& description);
    virtual ~VulkanFramebuffer() override;

private:
    RHIFramebufferDescription description_;
    VkFramebuffer handle_;
    VulkanContext& context_;
};

using RHIVulkanFramebufferRef = SharedRef<VulkanFramebuffer>;


}  //namespace licht