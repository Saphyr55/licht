#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

class RHIVulkanRenderPass : public RHIRenderPass {
public:
    VkRenderPass& get_handle();

public:
    RHIVulkanRenderPass(VulkanContext& context)
        : context_(context) {
    }

private:
    VulkanContext& context_;
    VkRenderPass render_pass_;
};

using RHIVulkanRenderPassRef = SharedRef<RHIVulkanRenderPass>;

void vulkan_render_pass_init(VulkanContext& context, VkRenderPass* render_pass, const RHIRenderPassDescription& description);
void vulkan_render_pass_destroy(VulkanContext& context, VkRenderPass render_pass);

}  //namespace licht