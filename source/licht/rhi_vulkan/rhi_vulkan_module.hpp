#pragma once

#include "licht/rhi_vulkan/rhi_vulkan_exports.hpp"

namespace licht {

struct VulkanContext;

class LICHT_RHI_VULKAN_API RHIVulkanModule {
public:
    void initialize();

    void shutdown();

private:
    VulkanContext* context_;
};

}