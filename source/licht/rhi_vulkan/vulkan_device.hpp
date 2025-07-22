#pragma once

#include <vulkan/vulkan_core.h>
#include "licht/rhi_vulkan/vulkan_logical_device.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"

namespace licht {

class VulkanDevice {
public:
    void initialize();

    void destroy();

    void query_graphics_queue();

    void query_present_queue();

public:
    inline VulkanPhysicalDeviceInformation get_info() {
        return physical_device_.get_info();
    }

    inline VulkanLogicalDevice get_logical_device() {
        return logical_device_;
    }

    inline VulkanPhysicalDevice get_physical_device() {
        return physical_device_; 
    }

    inline VkPhysicalDevice get_physical_device_handle() {
        return physical_device_.get_handle();
    }

    inline VkDevice get_handle() {
        return logical_device_.get_handle();
    }

    inline VkAllocationCallbacks* get_allocator() {
        return device_allocator_;
    }

private:
    VkAllocationCallbacks* device_allocator_;
    VulkanLogicalDevice logical_device_;
    VulkanPhysicalDevice physical_device_;
};

}

