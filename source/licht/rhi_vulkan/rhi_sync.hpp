#pragma once

#include "licht/rhi/fence.hpp"
#include "licht/rhi/semaphore.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanFence : public RHIFence {
public:
    VkFence get_handle() {
        return fence_;
    }

public:
    RHIVulkanFence(VkFence fence)
        : fence_(fence) {
    }

    RHIVulkanFence() = default;
    ~RHIVulkanFence() = default;


private:
    VkFence fence_;
};

class RHIVulkanSemaphore : public RHISemaphore {
public:
    VkSemaphore& get_handle() {
        return semaphore_;
    }

public:
    RHIVulkanSemaphore(VkSemaphore semaphore)
        : semaphore_(semaphore) {
    }

    RHIVulkanSemaphore() = default;
    ~RHIVulkanSemaphore() = default;

private:
    VkSemaphore semaphore_;
};

}  //namespace licht