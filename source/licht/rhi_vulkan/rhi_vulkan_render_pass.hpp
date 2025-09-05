#pragma once

#include "licht/rhi/render_pass.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

class RHIVulkanRenderPass : public RHIRenderPass {
public:
    virtual void begin() override;

    virtual void end() override;

    VkRenderPass& get_handle();

private:
    VkRenderPass render_pass_;
};

}