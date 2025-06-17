#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/platform/dynamic_library.hpp"
#include "licht/platform/window_handle.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_context.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_exports.hpp"

namespace licht {

class LICHT_RHI_VULKAN_API RHIVulkanModule {
public:
    void initialize();

    void shutdown();

private:
    RHIVulkanContext* context_;
};

}