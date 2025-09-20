#pragma once

#include <vulkan/vulkan_core.h>

#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"

namespace licht {

class VulkanInstance;

struct VulkanPhysicalDeviceInformation {
    VkPhysicalDeviceMemoryProperties memory_properties;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    Array<VkQueueFamilyProperties> queue_families;

    uint32 graphics_queue_index = UINT32_MAX;
    uint32 present_queue_index = UINT32_MAX;

    bool is_suitable;
};

class VulkanPhysicalDeviceSelector {
public:
    bool select_physical_device();

    const VulkanPhysicalDeviceInformation& query_info();

    VkPhysicalDeviceProperties query_properties();

    VkPhysicalDeviceFeatures query_features();

    Array<VkQueueFamilyProperties> query_queue_families();

    bool select_queue_families();

    bool is_valid_queue_family(const VkQueueFamilyProperties& queue_family_properties, int32 queue_family_index);

    bool is_properties_suitable() const;

    bool is_features_suitable() const;

    bool check_extension_support(const Array<StringRef>& extensions);

public:
    inline const VulkanPhysicalDeviceInformation& get_info() {
        return info_;
    }

public:
    VulkanPhysicalDeviceSelector(VulkanContext& context, const Array<StringRef>& extensions);
    ~VulkanPhysicalDeviceSelector() = default;

private:
    Array<StringRef> extensions_;
    VulkanPhysicalDeviceInformation info_;
    VulkanContext& context_;
};

}  // namespace licht
