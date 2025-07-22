#include "vulkan_logical_device.hpp"

#include <vulkan/vulkan_core.h>

#include "licht/core/collection/array.hpp"
#include "licht/core/collection/hash_map.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/allocator.hpp"
#include "licht/core/string/format.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"

namespace licht {

VulkanLogicalDevice::VulkanLogicalDevice(VulkanDevice& p_device, VulkanPhysicalDevice& p_physical_device)
    : device_(p_device)
    , physical_device_(p_physical_device) {
}

void VulkanLogicalDevice::initialize() {
    LCHECK_MSG(physical_device_.get_info().is_suitable, "Physical device is not suitable for Vulkan operations.")

    float32 queue_priority = 1.0f;  // [0.0, 1.0]

    uint32 graphics_queue_index = physical_device_.get_info().graphics_queue_index;
    uint32 present_queue_index = physical_device_.get_info().present_queue_index;

    // TODO: Use a hash set.
    HashMap<uint32, uint32> queue_famillies = {{graphics_queue_index, 0}, {present_queue_index, 0}};

    Array<VkDeviceQueueCreateInfo> device_queue_create_infos;
    for (auto& [queue_index, _] : queue_famillies) {
        VkDeviceQueueCreateInfo device_queue_create_info = {};
        device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_info.pNext = VK_NULL_HANDLE;
        device_queue_create_info.queueFamilyIndex = queue_index;
        device_queue_create_info.queueCount = 1;
        device_queue_create_info.pQueuePriorities = &queue_priority;
        device_queue_create_infos.append(device_queue_create_info);
    }

    VkPhysicalDeviceFeatures physical_device_features = {};

    Array<const char*> physical_device_extensions;
    physical_device_extensions.resize(g_physical_device_extensions.size());

    for (int32 i = 0; i < g_physical_device_extensions.size(); i++) {
        physical_device_extensions[i] = g_physical_device_extensions[i].data();
    }

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = VK_NULL_HANDLE;
    device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
    device_create_info.queueCreateInfoCount = static_cast<uint32>(device_queue_create_infos.size());
    device_create_info.pEnabledFeatures = &physical_device_features;
    device_create_info.ppEnabledExtensionNames = physical_device_extensions.data();
    device_create_info.enabledExtensionCount = static_cast<uint32>(physical_device_extensions.size());

    LLOG_INFO("[Vulkan]", "Creating Vulkan logical device with the following queue families:");
    for (auto& [queue_index, _] : queue_famillies) {
        LLOG_INFO("[Vulkan]", vformat("Queue Family Index: %s", queue_index));
    }

    LLOG_INFO("[Vulkan]", "Enabled Physical Device Extensions:");
    for (int32 i = 0; i < physical_device_extensions.size(); i++) {
        LLOG_INFO("[Vulkan]", vformat("Extension: %s", physical_device_extensions[i]));
    }

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateDevice(physical_device_.get_handle(), &device_create_info, device_.get_allocator(), &handle_));
}

void VulkanLogicalDevice::destroy() {

    LLOG_INFO("[Vulkan]", "Destroying Vulkan logical device.");

    VulkanAPI::lvkDestroyDevice(handle_, device_.get_allocator());
    handle_ = VK_NULL_HANDLE;

    LLOG_INFO("[Vulkan]", "Vulkan logical device destroyed.");
}

}  //namespace licht
