#pragma once

#include <vulkan/vulkan_core.h>

#include "licht/core/string/string_ref.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"
#include "licht/rhi_vulkan/vulkan_queue.hpp"

namespace licht {

class VulkanDevice;

// TODO: Make it not global.
static const Array<StringRef> g_physical_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

class VulkanLogicalDevice {
public:
    void initialize();

    void destroy();

public:
    inline VkDevice get_handle() {
        return handle_;
    }

    VulkanLogicalDevice(VulkanDevice& p_device, VulkanPhysicalDevice& p_physical_device);

private:
    VulkanDevice& device_;
    VkDevice handle_;
    VulkanPhysicalDevice& physical_device_;
};

}  //namespace licht