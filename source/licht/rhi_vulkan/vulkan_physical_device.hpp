#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>

#include "licht/core/collection/array.hpp"
#include "licht/core/collection/hash_map.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

class VulkanInstance;

struct VulkanPhysicalDeviceInformation {
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    Array<VkQueueFamilyProperties> queue_families;

    uint32 graphics_queue_index = UINT32_MAX;
    uint32 present_queue_index = UINT32_MAX;

    bool is_suitable;
};

class VulkanPhysicalDevice {
public:
    bool select_physical_device();
    
    const VulkanPhysicalDeviceInformation& query_info();

    VkPhysicalDeviceProperties query_properties();

    VkPhysicalDeviceFeatures query_features();

    Array<VkQueueFamilyProperties> query_queue_families();

    bool select_queue_families();

    bool is_valid_queue_family(const VkQueueFamilyProperties& p_queue_family_properties, int32 p_queue_family_index);

    bool is_properties_suitable() const;

    bool is_features_suitable() const;

    bool check_extension_support(const Array<StringRef>& p_extensions);

public:
    inline VkPhysicalDevice get_handle() {
        return handle_;
    }

    inline const VulkanPhysicalDeviceInformation& get_info() {
        return info_;
    }

    inline bool is_valid() {
        return handle_ != VK_NULL_HANDLE;
    }

public:
    VulkanPhysicalDevice(VulkanInstance& p_instance, VkSurfaceKHR p_surfarce, const Array<StringRef>& p_extensions);
    ~VulkanPhysicalDevice() = default;

private:
    Array<StringRef> extensions_;
    VulkanPhysicalDeviceInformation info_;
    VkPhysicalDevice handle_;
    VkSurfaceKHR surface_;
    VulkanInstance& instance_;
};

} // namespace licht
