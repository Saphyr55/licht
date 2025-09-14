#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/core/platform/dynamic_library.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"

#include <vulkan/vulkan_core.h>

#define LICHT_VULKAN_CHECK(Expr)                                                                                                   \
    {                                                                                                                              \
        VkResult result = (Expr);                                                                                                  \
        LLOG_FATAL_WHEN(result != VK_SUCCESS, "[Vulkan]", vformat("failed with the result '%s'", vulkan_string_of_result(result))) \
    }

namespace licht {

static const Array<StringRef> g_physical_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

class VulkanContext {
public:
    SharedRef<DynamicLibrary> library;

    VkInstance instance = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VulkanPhysicalDeviceInformation physical_device_info;

    SharedRef<RHIVulkanRenderSurface> surface;

    VkDebugUtilsMessengerEXT debug_utils_messenger = VK_NULL_HANDLE;
    VkAllocationCallbacks* allocator = nullptr;
};

// Vulkan Context functions.
void vulkan_context_initialize(VulkanContext& context, void* window_handle);
void vulkan_context_destroy(VulkanContext& context);

// Debug messenger.
void vulkan_debug_messenger_init(VulkanContext& p_context);
void vulkan_debug_messenger_destroy(VulkanContext& p_context);

// Vulkan Instance functions.
void vulkan_instance_initialize(VulkanContext& context);
void vulkan_instance_destroy(VulkanContext& context);

// Device
void vulkan_device_initialize(VulkanContext& context, VulkanPhysicalDeviceSelector& physical_device_selector);
void vulkan_device_destroy(VulkanContext& context);

// Queue
uint32 vulkan_query_queue_family_index(VulkanContext& context, RHIQueueType p_type);
VkQueue vulkan_query_queue(VulkanContext& context, RHIQueueType p_type);

// String convertion
const char* vulkan_string_of_present_mode(VkPresentModeKHR present_mode);
const char* vulkan_string_of_result(VkResult result);

// 
VkFormat vulkan_format_get(RHIFormat format);
RHIFormat rhi_format_get(VkFormat format);

}  //namespace licht