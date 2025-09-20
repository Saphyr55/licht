#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/platform/dynamic_library.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"

#include <vulkan/vulkan_core.h>

#define LICHT_VULKAN_CHECK(Expr)                                                                                                                                     \
    {                                                                                                                                                                \
        VkResult __licht_vulkan_result__ = (Expr);                                                                                                                   \
        LLOG_FATAL_WHEN(__licht_vulkan_result__ != VK_SUCCESS, "[Vulkan]", vformat("failed with the result '%s'", vulkan_string_of_result(__licht_vulkan_result__))) \
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

// String conversion
const char* vulkan_string_of_present_mode(VkPresentModeKHR present_mode);
const char* vulkan_string_of_result(VkResult result);

// Converts RHIFormat to VkFormat and vice versa.
VkFormat vulkan_format_get(RHIFormat format);
RHIFormat rhi_format_get(VkFormat format);

// Converts RHIAccessMode to VkSharingMode and vice versa.
RHIAccessMode rhi_access_mode_get(VkSharingMode mode);
VkSharingMode vulkan_sharing_mode_get(RHIAccessMode mode);

inline RHIBufferUsage operator|(RHIBufferUsage a, RHIBufferUsage b) {
    return static_cast<RHIBufferUsage>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline RHIBufferUsage operator&(RHIBufferUsage a, RHIBufferUsage b) {
    return static_cast<RHIBufferUsage>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline RHIBufferUsage& operator|=(RHIBufferUsage& a, RHIBufferUsage b) {
    a = a | b;
    return a;
}

inline RHIBufferUsage& operator&=(RHIBufferUsage& a, RHIBufferUsage b) {
    a = a & b;
    return a;
}

inline bool rhi_buffer_usage_any(RHIBufferUsage usage) {
    return static_cast<uint8_t>(usage) != 0;
}

inline VkVertexInputRate vulkan_vertex_input_rate_get(RHIVertexInputRate rate) {
    switch (rate) {
        case RHIVertexInputRate::Vertex:
            return VK_VERTEX_INPUT_RATE_VERTEX;
        case RHIVertexInputRate::Instance:
            return VK_VERTEX_INPUT_RATE_INSTANCE;
        default:
            return VK_VERTEX_INPUT_RATE_VERTEX;
    }
}

inline RHIVertexInputRate rhi_vertex_input_rate_get(VkVertexInputRate rate) {
    switch (rate) {
        case VK_VERTEX_INPUT_RATE_VERTEX:
            return RHIVertexInputRate::Vertex;
        case VK_VERTEX_INPUT_RATE_INSTANCE:
            return RHIVertexInputRate::Instance;
        default:
            return RHIVertexInputRate::Vertex;
    }
}

inline bool rhi_buffer_usage_has(RHIBufferUsage value, RHIBufferUsage flag) {
    return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
}

inline VkBufferUsageFlags vulkan_buffer_usage_get(RHIBufferUsage usage) {
    VkBufferUsageFlags flags = 0;

    if (rhi_buffer_usage_has(usage, RHIBufferUsage::Vertex)) {
        flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    if (rhi_buffer_usage_has(usage, RHIBufferUsage::Index)) {
        flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }
    if (rhi_buffer_usage_has(usage, RHIBufferUsage::Uniform)) {
        flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    if (rhi_buffer_usage_has(usage, RHIBufferUsage::Storage)) {
        flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    if (rhi_buffer_usage_has(usage, RHIBufferUsage::TransferSrc)) {
        flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }
    if (rhi_buffer_usage_has(usage, RHIBufferUsage::TransferDst)) {
        flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    return flags;
}

inline RHIBufferUsage rhi_buffer_usage_get(VkBufferUsageFlags usage) {
    RHIBufferUsage flags = RHIBufferUsage::None;

    if (usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
        flags |= RHIBufferUsage::Vertex;
    }
    if (usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
        flags |= RHIBufferUsage::Index;
    }
    if (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        flags |= RHIBufferUsage::Uniform;
    }
    if (usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        flags |= RHIBufferUsage::Storage;
    }
    if (usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) {
        flags |= RHIBufferUsage::TransferSrc;
    }
    if (usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT) {
        flags |= RHIBufferUsage::TransferDst;
    }

    return flags;
}
}  //namespace licht