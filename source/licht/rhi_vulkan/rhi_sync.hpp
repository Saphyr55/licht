#pragma once

#include "licht/rhi/fence.hpp"
#include "licht/rhi/semaphore.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanFence : public RHIFence {
public:
    VkFence& get_handle() {
        return fence_;
    }

    virtual bool is_signaled() {
        return signaled_;
    }

    inline void set_signaled(bool signaled) {
        signaled_ = signaled;
    }

public:
    RHIVulkanFence(VkFence fence, bool signaled)
        : fence_(fence)
        , signaled_(signaled) {
    }

    RHIVulkanFence() = default;
    ~RHIVulkanFence() = default;


private:
    bool signaled_;
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