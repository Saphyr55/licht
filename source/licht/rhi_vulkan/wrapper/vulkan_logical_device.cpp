#include "vulkan_logical_device.hpp"

#include <vulkan/vulkan_core.h>

#include "licht/core/collection/array.hpp"
#include "licht/core/collection/hash_map.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/core/string/format.hpp"
#include "licht/core/memory/allocator.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_context.hpp"

namespace licht {

void vulkan_logical_device_init(VulkanContext* p_context) {

    LCHECK(p_context)
    LCHECK_MSG(p_context->physical_device.is_suitable, "Physical device is not suitable for Vulkan operations.")

    float32 queue_priority = 1.0f;  // [0.0, 1.0]

    uint32 graphics_queue_index = p_context->physical_device.graphics_queue_index;
    uint32 present_queue_index = p_context->physical_device.present_queue_index;

    // TODO: Use a hash set.
    HashMap<uint32, uint32> queue_famillies = { {graphics_queue_index, 0}, {present_queue_index, 0} };

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

    LICHT_VULKAN_CHECK(p_context->rhi.licht_vkCreateDevice(p_context->physical_device.handle, &device_create_info, p_context->allocator, &p_context->device));
}

void vulkan_queues_init(VulkanContext* p_context) {
    LCHECK(p_context)

    uint32 graphics_queue_index = p_context->physical_device.graphics_queue_index;
    uint32 present_queue_index = p_context->physical_device.present_queue_index;
    
    p_context->rhi.licht_vkGetDeviceQueue(p_context->device, graphics_queue_index, 0, &p_context->graphics_queue);
    p_context->rhi.licht_vkGetDeviceQueue(p_context->device, present_queue_index, 0, &p_context->present_queue);
}

void vulkan_logical_device_destroy(VulkanContext* p_context) {
    LCHECK(p_context)

    LLOG_INFO("[Vulkan]", "Destroying Vulkan logical device.");
    
    p_context->rhi.licht_vkDestroyDevice(p_context->device, p_context->allocator);
    p_context->device = VK_NULL_HANDLE;

    LLOG_INFO("[Vulkan]", "Vulkan logical device destroyed.");
}

}  //namespace licht
