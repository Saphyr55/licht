#pragma once

#include <vulkan/vulkan_core.h>

#include "licht/core/collection/array.hpp"
#include "licht/core/collection/hash_map.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi_vulkan/vulkan_logical_device.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"
#include "licht/rhi_vulkan/vulkan_queue.hpp"

namespace licht {

class VulkanInstance;

class VulkanDevice {
public:
    void initialize();

    void destroy();

    VulkanQueue& query_queue(VulkanQueueFamilyType p_type);

    uint32 query_queue_family_index(VulkanQueueFamilyType p_type);

public:
    inline const VulkanPhysicalDeviceInformation& get_info() {
        return physical_device_.get_info();
    }

    inline VulkanPhysicalDevice get_physical_device() {
        return physical_device_; 
    }

    inline VkPhysicalDevice get_physical_device_handle() {
        return physical_device_.get_handle();
    }

    inline VkDevice get_handle() {
        return handle_;
    }

    inline VkAllocationCallbacks* get_allocator() {
        return device_allocator_;
    }

public:
    VulkanDevice(VulkanInstance& p_instance, VkSurfaceKHR p_surfarce, const Array<StringRef>& p_extensions);

private:
    HashMap<VulkanQueueFamilyType, VulkanQueue> queues_;
    VkAllocationCallbacks* device_allocator_ = nullptr;
    VulkanPhysicalDevice physical_device_;
    VkDevice handle_;
};

}

