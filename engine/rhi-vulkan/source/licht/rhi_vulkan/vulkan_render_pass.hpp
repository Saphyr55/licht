#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

class VulkanRenderPass : public RHIRenderPass {
public:
    inline virtual const RHIRenderPassDescription& get_description() const override {
        return description_;
    }

    VkRenderPass& get_handle() {
        return render_pass_;
    }

    const VkRenderPass& get_handle() const {
        return render_pass_;
    }

    void initialize();

    void destroy();

public:
    VulkanRenderPass(VulkanContext& context, const RHIRenderPassDescription& description)
        : context_(context)
        , description_(description)
        , render_pass_(VK_NULL_HANDLE) {
    }

private:
    VulkanContext& context_;
    RHIRenderPassDescription description_;
    VkRenderPass render_pass_;
};

using RHIVulkanRenderPassRef = SharedRef<VulkanRenderPass>;

}  //namespace licht