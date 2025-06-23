#pragma once

#include "licht/rhi_vulkan/rhi_vulkan_exports.hpp"

namespace licht {

struct VulkanContext;

class LICHT_RHI_VULKAN_API RHIVulkanModule {
public:
    void initialize();

    void tick();

    void shutdown();

    inline void update_resized() {
        window_resized_ = true;
    }

private:
    VulkanContext* context_;
    bool window_resized_ = false;
};

}