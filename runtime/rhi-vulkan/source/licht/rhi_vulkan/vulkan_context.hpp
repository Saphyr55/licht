#pragma once

#include "licht/core/function/function_ref.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/platform/dynamic_library.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"
#include "licht/rhi_vulkan/vulkan_render_surface.hpp"

#include <vulkan/vulkan_core.h>

#define LICHT_VULKAN_CHECK(Expr)                                                                                                                                     \
    do {                                                                                                                                                             \
        VkResult __licht_vulkan_result__ = (Expr);                                                                                                                   \
        LLOG_FATAL_WHEN(__licht_vulkan_result__ != VK_SUCCESS, "[Vulkan]", vformat("Failed with the result '%s'", vulkan_string_of_result(__licht_vulkan_result__))) \
    } while (false)

namespace licht {

static const Array<StringRef> g_physical_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct VulkanContext {
    SharedRef<DynamicLibrary> library;

    VkInstance instance = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VulkanPhysicalDeviceInformation physical_device_info;

    Array<RHICommandQueueRef> command_queues;

    SharedRef<VulkanRenderSurface> surface;

    VkDebugUtilsMessengerEXT debug_utils_messenger = VK_NULL_HANDLE;
    VkAllocationCallbacks* allocator = nullptr;
};

VulkanContext& vulkan_context_get();

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

// String conversion
const char* vulkan_string_of_present_mode(VkPresentModeKHR present_mode);
const char* vulkan_string_of_result(VkResult result);

// Queues functions.
RHIQueueType vulkan_queue_type(VulkanContext& context, uint32 queue_family_index);
Array<uint32> vulkan_query_queue_family_indices(VulkanContext& context, FunctionRef<bool(const VkQueueFamilyProperties&, uint32)> predicate = [](auto&, uint32) { return true; });
VkQueue vulkan_query_queue(VulkanContext& context, uint32 family_queue_index, uint32 queue_index);
bool vulkan_queue_present_support(VulkanContext& context, uint32 queue_family_index);

// Converts RHIAccessMode to VkSharingMode and vice versa.
RHISharingMode rhi_access_mode_get(VkSharingMode mode);
VkSharingMode vulkan_sharing_mode_get(RHISharingMode mode);

// Converts RHIFormat to VkFormat and vice versa.
VkFormat vulkan_format_get(RHIFormat format);
RHIFormat rhi_format_get(VkFormat format);

inline VkMemoryPropertyFlags vulkan_memory_usage_get(RHIMemoryUsage memory_usage) {
    VkMemoryPropertyFlags properties = 0;
    if (memory_usage == RHIMemoryUsage::Device) {
        properties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    } else if (memory_usage == RHIMemoryUsage::Host) {
        properties |= (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
    return properties;
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

inline bool rhi_buffer_usage_has(RHIBufferUsageFlags value, RHIBufferUsageFlags flag) {
    return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
}

inline bool rhi_texture_usage_has(RHITextureUsageFlags value, RHITextureUsageFlags flag) {
    return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
}

inline VkImageLayout vulkan_texture_layout_get(RHITextureLayout layout) {
    switch (layout) {
        case RHITextureLayout::Undefined:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        case RHITextureLayout::TransferDst:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case RHITextureLayout::TransferSrc:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case RHITextureLayout::ShaderReadOnly:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case RHITextureLayout::ColorAttachment:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case RHITextureLayout::DepthStencilAttachment:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case RHITextureLayout::General:
            return VK_IMAGE_LAYOUT_GENERAL;
        default:
            return VK_IMAGE_LAYOUT_UNDEFINED;
    }
}

inline RHITextureLayout rhi_texture_layout_get(VkImageLayout layout) {
    switch (layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            return RHITextureLayout::Undefined;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return RHITextureLayout::TransferDst;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return RHITextureLayout::TransferSrc;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return RHITextureLayout::ShaderReadOnly;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return RHITextureLayout::ColorAttachment;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return RHITextureLayout::DepthStencilAttachment;
        case VK_IMAGE_LAYOUT_GENERAL:
            return RHITextureLayout::General;
        default:
            return RHITextureLayout::Undefined;
    }
}

inline VkImageUsageFlags vulkan_texture_usage_get(RHITextureUsageFlags usage) {
    VkImageUsageFlags flags = 0;

    if (rhi_texture_usage_has(usage, RHITextureUsageFlags::ColorAttachment)) {
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
    if (rhi_texture_usage_has(usage, RHITextureUsageFlags::DepthStencilAttachment)) {
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    if (rhi_texture_usage_has(usage, RHITextureUsageFlags::Sampled)) {
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    if (rhi_texture_usage_has(usage, RHITextureUsageFlags::TransferSrc)) {
        flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    if (rhi_texture_usage_has(usage, RHITextureUsageFlags::TransferDst)) {
        flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    // TODO:
    // - VK_IMAGE_USAGE_STORAGE_BIT
    // - VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT

    return flags;
}

inline RHITextureUsageFlags rhi_texture_usage_get(VkImageUsageFlags usage) {
    RHITextureUsageFlags flags = static_cast<RHITextureUsageFlags>(0);

    if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        flags |= RHITextureUsageFlags::ColorAttachment;
    }
    if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        flags |= RHITextureUsageFlags::DepthStencilAttachment;
    }
    if (usage & VK_IMAGE_USAGE_SAMPLED_BIT) {
        flags |= RHITextureUsageFlags::Sampled;
    }
    if (usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        flags |= RHITextureUsageFlags::TransferSrc;
    }
    if (usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        flags |= RHITextureUsageFlags::TransferDst;
    }

    return flags;
}

inline VkBufferUsageFlags vulkan_buffer_usage_get(RHIBufferUsageFlags usage) {
    VkBufferUsageFlags flags = 0;

    if (rhi_buffer_usage_has(usage, RHIBufferUsageFlags::Vertex)) {
        flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    if (rhi_buffer_usage_has(usage, RHIBufferUsageFlags::Index)) {
        flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }
    if (rhi_buffer_usage_has(usage, RHIBufferUsageFlags::Uniform)) {
        flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    if (rhi_buffer_usage_has(usage, RHIBufferUsageFlags::Storage)) {
        flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    if (rhi_buffer_usage_has(usage, RHIBufferUsageFlags::TransferSrc)) {
        flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }
    if (rhi_buffer_usage_has(usage, RHIBufferUsageFlags::TransferDst)) {
        flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    return flags;
}

inline RHIBufferUsageFlags rhi_buffer_usage_get(VkBufferUsageFlags usage) {
    RHIBufferUsageFlags flags = RHIBufferUsageFlags::None;

    if (usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
        flags |= RHIBufferUsageFlags::Vertex;
    }
    if (usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
        flags |= RHIBufferUsageFlags::Index;
    }
    if (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        flags |= RHIBufferUsageFlags::Uniform;
    }
    if (usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        flags |= RHIBufferUsageFlags::Storage;
    }
    if (usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) {
        flags |= RHIBufferUsageFlags::TransferSrc;
    }
    if (usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT) {
        flags |= RHIBufferUsageFlags::TransferDst;
    }

    return flags;
}

inline uint32 vulkan_find_memory_type(VulkanContext& context, uint32 type_filter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    VulkanAPI::lvkGetPhysicalDeviceMemoryProperties(context.physical_device, &memory_properties);

    for (uint32 i = 0; i < memory_properties.memoryTypeCount; i++) {
        if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    LCRASH("Failed to find suitable Vulkan memory type for requested properties.")
    return 0;
}

inline VkDescriptorType vulkan_descriptor_type_get(RHIShaderResourceType type) {
    switch (type) {
        case RHIShaderResourceType::Uniform:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case RHIShaderResourceType::StorageBuffer:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case RHIShaderResourceType::StorageTexture:
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case RHIShaderResourceType::Sampler:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        default:
            LLOG_FATAL("[Vulkan]", "Unknown RHIShaderResourceType.");
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}

inline VkShaderStageFlags vulkan_shader_stage_get(RHIShaderStage stage) {
    VkShaderStageFlags flags = 0;
    if ((stage & RHIShaderStage::Vertex) == RHIShaderStage::Vertex) {
        flags |= VK_SHADER_STAGE_VERTEX_BIT;
    }

    if ((stage & RHIShaderStage::Fragment) == RHIShaderStage::Fragment) {
        flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    if ((stage & RHIShaderStage::Tesselation) == RHIShaderStage::Tesselation) {
        flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    }

    if ((stage & RHIShaderStage::Geometry) == RHIShaderStage::Geometry) {
        flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
    }

    if ((stage & RHIShaderStage::Compute) == RHIShaderStage::Compute) {
        flags |= VK_SHADER_STAGE_COMPUTE_BIT;
    }

    if (RHIShaderStage::AllGraphics == (stage & RHIShaderStage::AllGraphics)) {
        flags |= VK_SHADER_STAGE_ALL_GRAPHICS;
    }

    return flags;
}

inline VkFilter vulkan_filter_get(RHIFilter filter) {
    switch (filter) {
        case RHIFilter::Nearest:
            return VK_FILTER_NEAREST;
        case RHIFilter::Linear:
            return VK_FILTER_LINEAR;
        default:
            return VK_FILTER_NEAREST;
    }
    return VK_FILTER_NEAREST;
}

inline VkSamplerAddressMode vulkan_address_mode_get(RHISamplerAddressMode mode) {
    switch (mode) {
        case RHISamplerAddressMode::Repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case RHISamplerAddressMode::MirroredRepeat:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case RHISamplerAddressMode::ClampToEdge:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case RHISamplerAddressMode::ClampToBorder:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

inline VkImageAspectFlags vulkan_format_to_image_aspect(RHIFormat format) {
    switch (format) {
        case RHIFormat::D16:
        case RHIFormat::D24:
        case RHIFormat::D32:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        case RHIFormat::D24S8:
        case RHIFormat::D32S8:
            return (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        case RHIFormat::Undefined:
            return VK_IMAGE_ASPECT_NONE;
        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

inline VkQueueFlags vulkan_queue_type_get(RHIQueueType type) {
    VkQueueFlags flags = 0;

    if ((type & RHIQueueType::Graphics) == RHIQueueType::Graphics) {
        flags |= VK_QUEUE_GRAPHICS_BIT;
    }
    if ((type & RHIQueueType::Compute) == RHIQueueType::Compute) {
        flags |= VK_QUEUE_COMPUTE_BIT;
    }
    if ((type & RHIQueueType::Transfer) == RHIQueueType::Transfer) {
        flags |= VK_QUEUE_TRANSFER_BIT;
    }

    return flags;
}

}  //namespace licht