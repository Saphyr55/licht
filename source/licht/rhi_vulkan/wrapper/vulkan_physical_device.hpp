#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>

#include "licht/core/collection/array.hpp"
#include "licht/core/defines.hpp"

namespace licht {

struct VulkanContext;

struct VulkanPhysicalDevice {

    VkPhysicalDevice handle;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    Array<VkQueueFamilyProperties> queue_families;

    uint32 graphics_queue_index = UINT32_MAX;
    uint32 present_queue_index = UINT32_MAX;

    bool is_suitable;
};

bool vulkan_physical_device_init(VulkanContext* p_context);

} // namespace licht
