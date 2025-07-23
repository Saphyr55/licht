#include "licht/rhi_vulkan/vulkan_device.hpp"
#include "licht/core/string/format.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_instance.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_logical_device.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"
#include "licht/rhi_vulkan/vulkan_queue.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

VulkanDevice::VulkanDevice(VulkanInstance& p_instance, VkSurfaceKHR p_surfarce, const Array<StringRef>& p_extensions)
    : physical_device_(p_instance, p_surfarce, p_extensions)
    , device_allocator_(nullptr) {
}

void VulkanDevice::initialize() {
    physical_device_.select_physical_device();

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

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateDevice(physical_device_.get_handle(), &device_create_info, device_allocator_, &handle_));
}

void VulkanDevice::destroy() {
    LLOG_INFO("[Vulkan]", "Destroying Vulkan logical device.");

    VulkanAPI::lvkDestroyDevice(handle_, device_allocator_);
    handle_ = VK_NULL_HANDLE;

    LLOG_INFO("[Vulkan]", "Vulkan logical device destroyed.");
}

uint32 VulkanDevice::query_queue_family_index(VulkanQueueFamilyType p_type) {
    switch (p_type) {
        case VulkanQueueFamilyType::Graphics: {
            return physical_device_.get_info().graphics_queue_index;
        }
        case VulkanQueueFamilyType::Present: {
            return physical_device_.get_info().present_queue_index;
        }
        case VulkanQueueFamilyType::Unknown: {
            return 0;
        }
    }
}

VulkanQueue& VulkanDevice::query_queue(VulkanQueueFamilyType p_type) {
    
    if (queues_.contains(p_type)) {
        return queues_[p_type];
    }

    uint32 queue_family_index = query_queue_family_index(p_type);
    VulkanQueue queue(p_type, queue_family_index);
    VulkanAPI::lvkGetDeviceQueue(get_handle(), queue_family_index, 0, &queue.handle_);

    queues_.insert(p_type, std::move(queue));

    return queues_[p_type];
}

}  //namespace licht
