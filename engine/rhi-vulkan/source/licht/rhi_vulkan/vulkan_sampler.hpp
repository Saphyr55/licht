#pragma once

#include "licht/rhi/sampler.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class VulkanSampler : public RHISampler {
public:
    VkSampler& get_handle() {
        return handle_;
    }

    void initialize(const RHISamplerDescription& description);
    void destroy();

public:
    VulkanSampler() = default;
    ~VulkanSampler();

private:
    VkSampler handle_ = VK_NULL_HANDLE;
};

}