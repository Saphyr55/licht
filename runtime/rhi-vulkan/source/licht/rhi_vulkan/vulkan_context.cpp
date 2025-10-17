#include "licht/rhi_vulkan/vulkan_context.hpp"

#include "licht/core/function/function_ref.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/platform/dynamic_library.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/vulkan_command_queue.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"
#include "licht/rhi_vulkan/vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_shader_module.hpp"


#include <vulkan/vulkan_core.h>

namespace licht {

static VulkanContext g_context;

VulkanContext& vulkan_context_get() {
    return g_context;
}

void vulkan_device_initialize(VulkanContext& context, VulkanPhysicalDeviceSelector& physical_device_selector) {
    physical_device_selector.select_physical_device();
    context.physical_device_info = physical_device_selector.get_info();

    LCHECK_MSG(context.physical_device_info.is_suitable, "Physical device is not suitable for Vulkan operations.");

    float32 queue_priority = 1.0f;  // [0.0, 1.0]

    Array<uint32> queue_famillies = vulkan_query_queue_family_indices(context);

    LLOG_INFO("[Vulkan]", "Creating Vulkan logical device with the following queue families:");
    Array<VkDeviceQueueCreateInfo> device_queue_create_infos;
    for (uint32 queue_family_index : queue_famillies) {
        VkDeviceQueueCreateInfo device_queue_create_info = {};
        device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_info.pNext = VK_NULL_HANDLE;
        device_queue_create_info.queueFamilyIndex = queue_family_index;
        device_queue_create_info.queueCount = 1;
        device_queue_create_info.pQueuePriorities = &queue_priority;
        device_queue_create_infos.append(device_queue_create_info);

        const VkQueueFamilyProperties& props = context.physical_device_info.queue_families[queue_family_index];
        String type_str;
        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            type_str += "Graphics ";
        }
        if (props.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            type_str += "Compute ";
        }
        if (props.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            type_str += "Transfer ";
        }
        if (props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
            type_str += "Sparse ";
        }

        bool present_support = vulkan_queue_present_support(context, queue_family_index);
        LLOG_INFO("[Vulkan]", vformat("Queue Family Index: %d | Queues: %d | Types: %s| Present: %s",
                                      queue_family_index,
                                      props.queueCount,
                                      type_str.data(),
                                      present_support ? "Yes" : "No"));
    }

    VkPhysicalDeviceFeatures physical_device_features = {};
    physical_device_features.samplerAnisotropy = physical_device_selector.get_info().features.samplerAnisotropy;
    

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

    LLOG_INFO("[Vulkan]", "Enabled Physical Device Extensions:");
    for (int32 i = 0; i < physical_device_extensions.size(); i++) {
        LLOG_INFO("[Vulkan]", vformat("Extension: %s", physical_device_extensions[i]));
    }

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateDevice(context.physical_device, &device_create_info, context.allocator, &context.device));

    vulkan_device_api_load(context.device);
}

void vulkan_device_destroy(VulkanContext& context) {
    LLOG_INFO("[Vulkan]", "Destroying Vulkan logical device.");

    VulkanAPI::lvkDestroyDevice(context.device, context.allocator);
    context.device = VK_NULL_HANDLE;

    LLOG_INFO("[Vulkan]", "Vulkan logical device destroyed.");
}

void vulkan_context_initialize(VulkanContext& context, void* native_window) {
    LLOG_INFO("[Vulkan]", "Initializing Vulkan RHI context...");
    {
        context.library = vulkan_library_load();
        LLOG_FATAL_WHEN(!context.library, "[Vulkan]", "Failed to load Vulkan RHI library.");

        bool core_functions_loaded = vulkan_core_api_load(context.library);
        LLOG_FATAL_WHEN(!core_functions_loaded, "[Vulkan]", "Failed to load Vulkan RHI core functions.");

        vulkan_instance_initialize(context);

        vulkan_debug_messenger_init(context);

        context.surface = VulkanRenderSurface::create(context, native_window);
        context.surface->initialize();

        LCHECK(context.surface->get_handle());

        VulkanPhysicalDeviceSelector selector(context, g_physical_device_extensions);
        vulkan_device_initialize(context, selector);

        size_t family_queue_size = context.physical_device_info.queue_families.size();
        context.command_queues.resize(family_queue_size);

        for (size_t i = 0; i < family_queue_size; i++) {
            VkQueue queue = vulkan_query_queue(context, i, 0);
            RHIQueueType type = vulkan_queue_type(context, i);
            bool is_present_mode = vulkan_queue_present_support(context, i);
            context.command_queues[i] = new_ref<VulkanCommandQueue>(context, queue, type, i, is_present_mode);
        }
    }
}

void vulkan_context_destroy(VulkanContext& context) {
    vulkan_device_destroy(context);

    context.surface->destroy();

    vulkan_debug_messenger_destroy(context);

    vulkan_instance_destroy(context);

    if (context.library) {
        DynamicLibraryLoader::unload(context.library);
        LLOG_INFO("[Vulkan]", "Vulkan RHI context destroyed.");
    }
}

bool vulkan_queue_present_support(VulkanContext& context, uint32 queue_family_index) {
    VkBool32 is_present_support = false;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfaceSupportKHR(context.physical_device, queue_family_index, context.surface->get_handle(), &is_present_support));
    return is_present_support;
}

RHIQueueType vulkan_queue_type(VulkanContext& context, uint32 queue_family_index) {
    const VkQueueFamilyProperties& queue_family_properties = context.physical_device_info.queue_families[queue_family_index];

    if (queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        return RHIQueueType::Graphics;
    }

    if (queue_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
        return RHIQueueType::Compute;
    }

    if (queue_family_properties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
        return RHIQueueType::Transfer;
    }

    return RHIQueueType::Unknown;
}

Array<uint32> vulkan_query_queue_family_indices(VulkanContext& context, FunctionRef<bool(const VkQueueFamilyProperties&, uint32)> predicate) {
    VulkanPhysicalDeviceInformation& info = context.physical_device_info;
    Array<uint32> indices(info.queue_families.size());

    for (uint32 queue_family_index = 0; queue_family_index < info.queue_families.size(); queue_family_index++) {
        if (predicate(info.queue_families[queue_family_index], queue_family_index)) {
            indices.append(queue_family_index);
        }
    }

    return indices;
}

VkQueue vulkan_query_queue(VulkanContext& context, uint32 queue_family_index, uint32 queue_index) {
    VkQueue queue;
    VulkanAPI::lvkGetDeviceQueue(context.device, queue_family_index, queue_index, &queue);
    return queue;
}

const char* vulkan_string_of_present_mode(VkPresentModeKHR present_mode) {
    switch (present_mode) {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            return "VK_PRESENT_MODE_IMMEDIATE_KHR";
        case VK_PRESENT_MODE_MAILBOX_KHR:
            return "VK_PRESENT_MODE_MAILBOX_KHR";
        case VK_PRESENT_MODE_FIFO_KHR:
            return "VK_PRESENT_MODE_FIFO_KHR";
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            return "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
        case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
            return "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR";
        case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
            return "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR";
        default:
            return "VK_UNKNOWN_PRESENT_MODE";
    }
}

const char* vulkan_string_of_result(VkResult result) {
    switch (result) {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        case VK_NOT_READY:
            return "VK_NOT_READY";
        case VK_TIMEOUT:
            return "VK_TIMEOUT";
        case VK_EVENT_SET:
            return "VK_EVENT_SET";
        case VK_EVENT_RESET:
            return "VK_EVENT_RESET";
        case VK_INCOMPLETE:
            return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:
            return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:
            return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
            return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        default:
            return "VK_UNKNOWN_ERROR";
    }
}

RHIFormat rhi_format_get(VkFormat format) {
    switch (format) {
        // 8-bit
        case VK_FORMAT_R8_UNORM:
            return RHIFormat::R8;
        case VK_FORMAT_R8G8_UNORM:
            return RHIFormat::RG8;
        case VK_FORMAT_R8G8B8_UNORM:
            return RHIFormat::RGB8;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return RHIFormat::RGBA8;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return RHIFormat::BGRA8;

        case VK_FORMAT_R8_SRGB:
            return RHIFormat::R8sRGB;
        case VK_FORMAT_R8G8_SRGB:
            return RHIFormat::RG8sRGB;
        case VK_FORMAT_R8G8B8_SRGB:
            return RHIFormat::RGB8sRGB;
        case VK_FORMAT_R8G8B8A8_SRGB:
            return RHIFormat::RGBA8sRGB;
        case VK_FORMAT_B8G8R8A8_SRGB:
            return RHIFormat::BGRA8sRGB;

        // 16-bit
        case VK_FORMAT_R16_UNORM:
            return RHIFormat::R16;
        case VK_FORMAT_R16G16_UNORM:
            return RHIFormat::RG16;
        case VK_FORMAT_R16G16B16_UNORM:
            return RHIFormat::RGB16;
        case VK_FORMAT_R16G16B16A16_UNORM:
            return RHIFormat::RGBA16;

        case VK_FORMAT_R16_SFLOAT:
            return RHIFormat::R16Float;
        case VK_FORMAT_R16G16_SFLOAT:
            return RHIFormat::RG16Float;
        case VK_FORMAT_R16G16B16_SFLOAT:
            return RHIFormat::RGB16Float;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return RHIFormat::RGBA16Float;

        // 32-bit
        case VK_FORMAT_R32_UINT:
            return RHIFormat::R32Uint;
        case VK_FORMAT_R32G32_UINT:
            return RHIFormat::RG32Uint;
        case VK_FORMAT_R32G32B32_UINT:
            return RHIFormat::RGB32Uint;
        case VK_FORMAT_R32G32B32A32_UINT:
            return RHIFormat::RGBA32Uint;

        case VK_FORMAT_R32_SINT:
            return RHIFormat::R32Sint;
        case VK_FORMAT_R32G32_SINT:
            return RHIFormat::RG32Sint;
        case VK_FORMAT_R32G32B32_SINT:
            return RHIFormat::RGB32Sint;
        case VK_FORMAT_R32G32B32A32_SINT:
            return RHIFormat::RGBA32Sint;

        case VK_FORMAT_R32_SFLOAT:
            return RHIFormat::R32Float;
        case VK_FORMAT_R32G32_SFLOAT:
            return RHIFormat::RG32Float;
        case VK_FORMAT_R32G32B32_SFLOAT:
            return RHIFormat::RGB32Float;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return RHIFormat::RGBA32Float;

        // Packed
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            return RHIFormat::RGB10A2;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
            return RHIFormat::R11G11B10Float;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
            return RHIFormat::RGB9E5;

        // Depth / Stencil
        case VK_FORMAT_D16_UNORM:
            return RHIFormat::D16;
        case VK_FORMAT_X8_D24_UNORM_PACK32:
            return RHIFormat::D24;
        case VK_FORMAT_D32_SFLOAT:
            return RHIFormat::D32;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return RHIFormat::D24S8;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return RHIFormat::D32S8;

        // Compressed
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
            return RHIFormat::BC1;
        case VK_FORMAT_BC3_UNORM_BLOCK:
            return RHIFormat::BC3;
        case VK_FORMAT_BC5_UNORM_BLOCK:
            return RHIFormat::BC5;
        case VK_FORMAT_BC7_UNORM_BLOCK:
            return RHIFormat::BC7;

        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
            return RHIFormat::ETC2_RGB8;
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
            return RHIFormat::ETC2_RGBA8;

        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
            return RHIFormat::ASTC_4x4;
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
            return RHIFormat::ASTC_8x8;

        default:
            return RHIFormat::Undefined;
    }
}

VkFormat vulkan_format_get(RHIFormat format) {
    switch (format) {
        case RHIFormat::R8:
            return VK_FORMAT_R8_UNORM;
        case RHIFormat::RG8:
            return VK_FORMAT_R8G8_UNORM;
        case RHIFormat::RGB8:
            return VK_FORMAT_R8G8B8_UNORM;
        case RHIFormat::RGBA8:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case RHIFormat::BGRA8:
            return VK_FORMAT_B8G8R8A8_UNORM;

        case RHIFormat::R8sRGB:
            return VK_FORMAT_R8_SRGB;
        case RHIFormat::RG8sRGB:
            return VK_FORMAT_R8G8_SRGB;
        case RHIFormat::RGB8sRGB:
            return VK_FORMAT_R8G8B8_SRGB;
        case RHIFormat::RGBA8sRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case RHIFormat::BGRA8sRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;

        case RHIFormat::R16:
            return VK_FORMAT_R16_UNORM;
        case RHIFormat::RG16:
            return VK_FORMAT_R16G16_UNORM;
        case RHIFormat::RGB16:
            return VK_FORMAT_R16G16B16_UNORM;
        case RHIFormat::RGBA16:
            return VK_FORMAT_R16G16B16A16_UNORM;

        case RHIFormat::R16Float:
            return VK_FORMAT_R16_SFLOAT;
        case RHIFormat::RG16Float:
            return VK_FORMAT_R16G16_SFLOAT;
        case RHIFormat::RGB16Float:
            return VK_FORMAT_R16G16B16_SFLOAT;
        case RHIFormat::RGBA16Float:
            return VK_FORMAT_R16G16B16A16_SFLOAT;

        case RHIFormat::R32Uint:
            return VK_FORMAT_R32_UINT;
        case RHIFormat::RG32Uint:
            return VK_FORMAT_R32G32_UINT;
        case RHIFormat::RGB32Uint:
            return VK_FORMAT_R32G32B32_UINT;
        case RHIFormat::RGBA32Uint:
            return VK_FORMAT_R32G32B32A32_UINT;

        case RHIFormat::R32Sint:
            return VK_FORMAT_R32_SINT;
        case RHIFormat::RG32Sint:
            return VK_FORMAT_R32G32_SINT;
        case RHIFormat::RGB32Sint:
            return VK_FORMAT_R32G32B32_SINT;
        case RHIFormat::RGBA32Sint:
            return VK_FORMAT_R32G32B32A32_SINT;

        case RHIFormat::R32Float:
            return VK_FORMAT_R32_SFLOAT;
        case RHIFormat::RG32Float:
            return VK_FORMAT_R32G32_SFLOAT;
        case RHIFormat::RGB32Float:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case RHIFormat::RGBA32Float:
            return VK_FORMAT_R32G32B32A32_SFLOAT;

        case RHIFormat::RGB10A2:
            return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        case RHIFormat::R11G11B10Float:
            return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        case RHIFormat::RGB9E5:
            return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;

        case RHIFormat::D16:
            return VK_FORMAT_D16_UNORM;
        case RHIFormat::D24:
            return VK_FORMAT_X8_D24_UNORM_PACK32;
        case RHIFormat::D32:
            return VK_FORMAT_D32_SFLOAT;
        case RHIFormat::D24S8:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case RHIFormat::D32S8:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;

        case RHIFormat::BC1:
            return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case RHIFormat::BC3:
            return VK_FORMAT_BC3_UNORM_BLOCK;
        case RHIFormat::BC5:
            return VK_FORMAT_BC5_UNORM_BLOCK;
        case RHIFormat::BC7:
            return VK_FORMAT_BC7_UNORM_BLOCK;

        case RHIFormat::ETC2_RGB8:
            return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
        case RHIFormat::ETC2_RGBA8:
            return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;

        case RHIFormat::ASTC_4x4:
            return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        case RHIFormat::ASTC_8x8:
            return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;

        case RHIFormat::Undefined:
        default:
            return VK_FORMAT_UNDEFINED;
    }
}

RHISharingMode rhi_access_mode_get(VkSharingMode mode) {
    switch (mode) {
        case VK_SHARING_MODE_EXCLUSIVE:
            return RHISharingMode::Private;
        case VK_SHARING_MODE_CONCURRENT:
            return RHISharingMode::Shared;
        default:
            return RHISharingMode::Private;
    }
}

VkSharingMode vulkan_sharing_mode_get(RHISharingMode mode) {
    switch (mode) {
        case RHISharingMode::Private:
            return VK_SHARING_MODE_EXCLUSIVE;
        case RHISharingMode::Shared:
            return VK_SHARING_MODE_CONCURRENT;
        default:
            return VK_SHARING_MODE_EXCLUSIVE;
    }
}

}  //namespace licht