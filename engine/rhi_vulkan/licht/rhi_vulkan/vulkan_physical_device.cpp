
#include "licht/core/trace/trace.hpp"

#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

VulkanPhysicalDeviceSelector::VulkanPhysicalDeviceSelector(VulkanContext& context, const Array<StringRef>& extensions)
    : context_(context), extensions_(extensions) {}

VkPhysicalDeviceProperties VulkanPhysicalDeviceSelector::query_properties() {
    VulkanAPI::lvkGetPhysicalDeviceProperties(context_.physical_device, &info_.properties);
    return info_.properties;
}

VkPhysicalDeviceFeatures VulkanPhysicalDeviceSelector::query_features() {
    VulkanAPI::lvkGetPhysicalDeviceFeatures(context_.physical_device, &info_.features);
    return info_.features;
}

bool VulkanPhysicalDeviceSelector::is_properties_suitable() const {
    return info_.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && info_.properties.limits.maxImageDimension2D >= 4096;
}

bool VulkanPhysicalDeviceSelector::is_features_suitable() const {
    return info_.features.geometryShader && info_.features.samplerAnisotropy;
}

bool VulkanPhysicalDeviceSelector::check_extension_support(const Array<StringRef>& extensions) {
    uint32 extension_count;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkEnumerateDeviceExtensionProperties(context_.physical_device, nullptr, &extension_count, nullptr));

    Array<VkExtensionProperties> available_extensions;
    available_extensions.resize(extension_count);
    LICHT_VULKAN_CHECK(VulkanAPI::lvkEnumerateDeviceExtensionProperties(context_.physical_device, nullptr, &extension_count, available_extensions.data()));

    Array<StringRef> physical_device_extensions = extensions;

    for (const VkExtensionProperties& extension : available_extensions) {
        physical_device_extensions.remove(extension.extensionName);
    }

    return physical_device_extensions.empty();
}

Array<VkQueueFamilyProperties> VulkanPhysicalDeviceSelector::query_queue_families() {
    uint32 queue_family_count = 0;
    VulkanAPI::lvkGetPhysicalDeviceQueueFamilyProperties(context_.physical_device, &queue_family_count, nullptr);
    if (queue_family_count == 0) {
        return false;
    }

    info_.queue_families.resize(queue_family_count);
    VulkanAPI::lvkGetPhysicalDeviceQueueFamilyProperties(context_.physical_device, &queue_family_count, info_.queue_families.data());

    return info_.queue_families;
}

bool VulkanPhysicalDeviceSelector::is_valid_queue_family(const VkQueueFamilyProperties& queue_family_properties, int32 queue_family_index) {
    VkBool32 is_present_support = false;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfaceSupportKHR(context_.physical_device, queue_family_index, context_.surface->get_handle(), &is_present_support));
    return (queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && is_present_support == VK_TRUE;
}

bool VulkanPhysicalDeviceSelector::select_queue_families() {
    for (uint32 queue_family_index = 0; queue_family_index < info_.queue_families.size(); queue_family_index++) {
        const VkQueueFamilyProperties& queue_family = info_.queue_families[queue_family_index];

        if (is_valid_queue_family(queue_family, queue_family_index)) {
            info_.graphics_queue_index = queue_family_index;
            info_.present_queue_index = queue_family_index;
            return true;
        }
    }

    return false;
}

const VulkanPhysicalDeviceInformation& VulkanPhysicalDeviceSelector::query_info() {
    query_properties();
    bool is_suitable_device_properties = is_properties_suitable();

    query_features();
    bool is_suitable_device_features = is_features_suitable();

    query_queue_families();
    bool is_suitable_queue_families = select_queue_families();

    bool is_suitable_extension_support = check_extension_support(g_physical_device_extensions);

    VulkanAPI::lvkGetPhysicalDeviceMemoryProperties(context_.physical_device, &info_.memory_properties);

    info_.is_suitable =
        is_suitable_device_properties &&
        is_suitable_device_features &&
        is_suitable_queue_families &&
        is_suitable_extension_support;

    return info_;
}

bool VulkanPhysicalDeviceSelector::select_physical_device() {
    VkInstance instance = context_.instance;

    uint32 device_count = 0;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkEnumeratePhysicalDevices(instance, &device_count, nullptr));
    if (device_count == 0) {
        LLOG_ERROR("[Vulkan]", "No physical devices found.");
        return false;
    }

    Array<VkPhysicalDevice> devices;
    devices.resize(device_count);
    LICHT_VULKAN_CHECK(VulkanAPI::lvkEnumeratePhysicalDevices(instance, &device_count, devices.data()));

    for (const VkPhysicalDevice& physical_device : devices) {
        context_.physical_device = physical_device;

        query_info();

        // Check if the device meets the requirements
        if (info_.is_suitable) {
            LLOG_INFO("[Vulkan]", "Physical device selected successfully.");
            LLOG_ERROR("[Vulkan]", "No suitable physical device found.");

            LLOG_INFO("[Vulkan]", "Selected Physical Device Properties:");
            LLOG_INFO("[Vulkan]", vformat("Device Name: %s", info_.properties.deviceName));
            LLOG_INFO("[Vulkan]", vformat("API Version: %u.%u.%u",
                                          VK_VERSION_MAJOR(info_.properties.apiVersion),
                                          VK_VERSION_MINOR(info_.properties.apiVersion),
                                          VK_VERSION_PATCH(info_.properties.apiVersion)));
            LLOG_INFO("[Vulkan]", vformat("Driver Version: %u", info_.properties.driverVersion));
            LLOG_INFO("[Vulkan]", vformat("Vendor ID: %u", info_.properties.vendorID));
            LLOG_INFO("[Vulkan]", vformat("Device ID: %u", info_.properties.deviceID));
            LLOG_INFO("[Vulkan]", vformat("Device Type: %d", info_.properties.deviceType));
            return true;
        }
    }

    
    return false;
}

}  //namespace licht