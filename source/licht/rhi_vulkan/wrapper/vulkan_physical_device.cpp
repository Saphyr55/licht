#include "vulkan_physical_device.hpp"
#include "licht/core/collection/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/string/format.hpp"
#include "licht/core/string/string.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/core/trace/trace.hpp"

#include "licht/rhi_vulkan/wrapper/vulkan_context.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_logical_device.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

bool physical_device_check_extension_support(VulkanContext* p_context,
                                             VulkanPhysicalDevice* p_physical_device,
                                             const Array<StringRef>& p_physical_device_extensions) {
    LCHECK(p_context)

    uint32 extension_count;
    LICHT_VULKAN_CHECK(p_context->rhi.licht_vkEnumerateDeviceExtensionProperties(p_physical_device->handle, nullptr, &extension_count, nullptr));

    Array<VkExtensionProperties> available_extensions(extension_count);
    LICHT_VULKAN_CHECK(p_context->rhi.licht_vkEnumerateDeviceExtensionProperties(p_physical_device->handle, nullptr, &extension_count, available_extensions.data()));

    Array<StringRef> physical_device_extensions = p_physical_device_extensions;

    for (const VkExtensionProperties& extension : available_extensions) {
        physical_device_extensions.remove(extension.extensionName);
    }

    return physical_device_extensions.empty();
}

bool physical_device_query_device_properties(VulkanContext* p_context, VulkanPhysicalDevice* p_physical_device) {
    LCHECK(p_context && p_physical_device)

    p_context->rhi.licht_vkGetPhysicalDeviceProperties(p_physical_device->handle, &p_physical_device->properties);
    bool is_suitable = p_physical_device->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                       p_physical_device->properties.limits.maxImageDimension2D >= 4096;
    return is_suitable;
}

bool physical_device_query_device_features(VulkanContext* p_context, VulkanPhysicalDevice* p_physical_device) {
    LCHECK(p_context && p_physical_device)

    p_context->rhi.licht_vkGetPhysicalDeviceFeatures(p_physical_device->handle, &p_physical_device->features);
    bool is_suitable = p_physical_device->features.geometryShader &&
           p_physical_device->features.samplerAnisotropy;
    return is_suitable;
}

bool physical_device_query_queue_families(VulkanContext* p_context, VulkanPhysicalDevice* p_physical_device) {
    LCHECK(p_context && p_physical_device)

    uint32 queue_family_count = 0;
    p_context->rhi.licht_vkGetPhysicalDeviceQueueFamilyProperties(p_physical_device->handle, &queue_family_count, nullptr);
    if (queue_family_count == 0) {
        return false;
    }

    p_physical_device->queue_families.resize(queue_family_count);
    p_context->rhi.licht_vkGetPhysicalDeviceQueueFamilyProperties(p_physical_device->handle, &queue_family_count, p_physical_device->queue_families.data());

    for (uint32 i = 0; i < queue_family_count; i++) {
        const VkQueueFamilyProperties& queue_family = p_physical_device->queue_families[i];

        VkBool32 is_present_support = false;
        LICHT_VULKAN_CHECK(p_context->rhi.licht_vkGetPhysicalDeviceSurfaceSupportKHR(p_physical_device->handle, i, p_context->surface, &is_present_support));

        if ((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) && is_present_support == VK_TRUE) {
            p_physical_device->graphics_queue_index = i;
            p_physical_device->present_queue_index = i;
            return true;
        }
    }

    return false;
}

bool physical_device_select(VulkanContext* p_context, VulkanPhysicalDevice* p_physical_device) {
    LCHECK(p_context && p_physical_device)

    uint32 device_count = 0;
    LICHT_VULKAN_CHECK(p_context->rhi.licht_vkEnumeratePhysicalDevices(p_context->instance, &device_count, nullptr));
    if (device_count == 0) {
        LLOG_ERROR("[Vulkan]", "No physical devices found.");
        return false;
    }

    Array<VkPhysicalDevice> devices(device_count);
    LICHT_VULKAN_CHECK(p_context->rhi.licht_vkEnumeratePhysicalDevices(p_context->instance, &device_count, devices.data()));

    for (const VkPhysicalDevice& physical_device : devices) {
        p_physical_device->handle = physical_device;

        // Query device properties.
        bool is_suitable_device_properties = physical_device_query_device_properties(p_context, p_physical_device);
        bool is_suitable_device_features = physical_device_query_device_features(p_context, p_physical_device);
        bool is_suitable_queue_families = physical_device_query_queue_families(p_context, p_physical_device);
        bool is_suitable_extension_support = physical_device_check_extension_support(p_context, p_physical_device, g_physical_device_extensions);

        p_physical_device->is_suitable =
            is_suitable_device_properties &&
            is_suitable_device_features &&
            is_suitable_queue_families &&
            is_suitable_extension_support;

        // Check if the device meets the requirements
        if (p_physical_device->is_suitable) {
            LLOG_INFO("[Vulkan]", "Physical device selected successfully.");
            return true;
        }
    }

    LLOG_ERROR("[Vulkan]", "No suitable physical device found.");
    return false;
}

bool vulkan_physical_device_init(VulkanContext* p_context) {
    LCHECK(p_context)

    bool seleted = physical_device_select(p_context, &p_context->physical_device);

    if (!seleted) {
        return false;
    }

    const VkPhysicalDeviceProperties& properties = p_context->physical_device.properties;
    LLOG_INFO("[Vulkan]", "Selected Physical Device Properties:");
    LLOG_INFO("[Vulkan]", vformat("Device Name: %s", properties.deviceName));
    LLOG_INFO("[Vulkan]", vformat("API Version: %u.%u.%u",
                                  VK_VERSION_MAJOR(properties.apiVersion),
                                  VK_VERSION_MINOR(properties.apiVersion),
                                  VK_VERSION_PATCH(properties.apiVersion)));
    LLOG_INFO("[Vulkan]", vformat("Driver Version: %u", properties.driverVersion));
    LLOG_INFO("[Vulkan]", vformat("Vendor ID: %u", properties.vendorID));
    LLOG_INFO("[Vulkan]", vformat("Device ID: %u", properties.deviceID));
    LLOG_INFO("[Vulkan]", vformat("Device Type: %d", properties.deviceType));

    return true;
}

}  //namespace licht