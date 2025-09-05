#pragma once

#include "licht/rhi/framebuffer.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

class RHIVulkanFramebuffer : public RHIFramebuffer {
public:
    VkFramebuffer& get_handle();

    virtual RHIFramebufferDescription get_description() override;

public:
    RHIVulkanFramebuffer(const RHIFramebufferDescription& p_description);

private:
    RHIFramebufferDescription description_;
    VkFramebuffer handle_;
};

}  //namespace licht