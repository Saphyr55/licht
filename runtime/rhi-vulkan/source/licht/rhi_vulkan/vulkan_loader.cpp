#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/platform/dynamic_library.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#define LICHT_LOAD_RHI_CORE_FUNCTION(Name)                                        \
    VulkanAPI::LICHT_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) = reinterpret_cast<    \
        LICHT_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name)>(                             \
        ::licht::DynamicLibraryLoader::load_function(library, #Name)); \
    if (VulkanAPI::l##Name == nullptr) {                                          \
        LLOG_ERROR("[Vulkan]", "Failed to load Vulkan function: " #Name);         \
    }

#define LICHT_LOAD_RHI_INSTANCE_FUNCTION(Name)                                                             \
    VulkanAPI::LICHT_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) = (LICHT_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name)) \
        VulkanAPI::lvkGetInstanceProcAddr(instance, #Name);                                     \
    if (VulkanAPI::LICHT_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) == nullptr) {                               \
        LLOG_ERROR("[Vulkan]", "Failed to load Vulkan instance function: " #Name);                         \
    }

#define LICHT_LOAD_RHI_DEVICE_FUNCTION(Name)                                     \
    VulkanAPI::LICHT_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) = reinterpret_cast<   \
        LICHT_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name)>(                            \
        VulkanAPI::lvkGetDeviceProcAddr(device, #Name));              \
    if (VulkanAPI::LICHT_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) == nullptr) {     \
        LLOG_ERROR("[Vulkan]", "Failed to load Vulkan device function: " #Name); \
    }

namespace licht {

// Core Vulkan functions.
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetInstanceProcAddr);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkEnumerateInstanceVersion);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkEnumerateInstanceExtensionProperties);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkEnumerateInstanceLayerProperties);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateInstance);

// Instance functions.
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetDeviceProcAddr);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyInstance);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkEnumeratePhysicalDevices);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceProperties);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceProperties2);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceFeatures);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceFeatures2);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceMemoryProperties);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceQueueFamilyProperties);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceFormatProperties);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceSurfaceFormatsKHR);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceSurfacePresentModesKHR);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceSurfaceSupportKHR)
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkEnumerateDeviceExtensionProperties);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateDevice);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroySurfaceKHR);

// Device functions.
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetDeviceQueue);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDeviceWaitIdle);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateCommandPool);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyCommandPool);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyDevice);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateSwapchainKHR);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroySwapchainKHR);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetSwapchainImagesKHR);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateImage);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateImageView);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyImage);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyImageView);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateRenderPass);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyRenderPass);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateFramebuffer);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyFramebuffer);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetImageMemoryRequirements);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkAllocateMemory);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkFreeMemory);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkAllocateCommandBuffers);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkFreeCommandBuffers);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkBeginCommandBuffer);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkEndCommandBuffer);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkResetCommandBuffer);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkBindImageMemory);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateSemaphore);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroySemaphore);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateFence);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyFence);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkWaitForFences);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkAcquireNextImageKHR);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkResetFences);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateDescriptorSetLayout);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyDescriptorSetLayout);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateDescriptorPool);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyDescriptorPool);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateShaderModule);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyShaderModule);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateSampler);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroySampler);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateBuffer);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyBuffer);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkGetBufferMemoryRequirements);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkBindBufferMemory);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkMapMemory);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkUnmapMemory);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkFlushMappedMemoryRanges);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreatePipelineLayout);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyPipelineLayout);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCreateGraphicsPipelines);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkDestroyPipeline);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdBindPipeline);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkAllocateDescriptorSets);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkFreeDescriptorSets);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkUpdateDescriptorSets);

LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdPipelineBarrier);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdBlitImage);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdCopyBuffer);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdCopyBufferToImage);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdCopyImageToBuffer);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdExecuteCommands);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetViewport);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetScissor);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetFrontFace);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetCullMode);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetStencilTestEnable);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetDepthTestEnable);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetDepthWriteEnable);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetStencilReference);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetStencilOp);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdBeginRendering);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdEndRendering);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetStencilCompareMask);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetStencilWriteMask);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdClearColorImage);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdClearDepthStencilImage);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetPrimitiveTopology);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdPushConstants);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdBindDescriptorSets);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdBindVertexBuffers);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdBindIndexBuffer);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdDraw);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdDrawIndexed);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdBeginRenderPass);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkCmdEndRenderPass);

LICHT_DEFINE_RHI_FUNCTION_IMPL(vkQueueSubmit);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkQueueWaitIdle);
LICHT_DEFINE_RHI_FUNCTION_IMPL(vkQueuePresentKHR);

SharedRef<DynamicLibrary> vulkan_library_load() {

#if defined(_WIN32)
    return DynamicLibraryLoader::load("vulkan-1.dll");
#endif

    return SharedRef<DynamicLibrary>(nullptr);
}

bool vulkan_core_api_load(SharedRef<DynamicLibrary> library) {
    LICHT_LOAD_RHI_CORE_FUNCTION(vkGetInstanceProcAddr);
    LICHT_LOAD_RHI_CORE_FUNCTION(vkEnumerateInstanceVersion);
    LICHT_LOAD_RHI_CORE_FUNCTION(vkEnumerateInstanceExtensionProperties);
    LICHT_LOAD_RHI_CORE_FUNCTION(vkEnumerateInstanceLayerProperties);
    LICHT_LOAD_RHI_CORE_FUNCTION(vkCreateInstance);

    return true;
}

bool vulkan_instance_api_load(VkInstance instance) {
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetDeviceProcAddr);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkDestroyInstance);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkEnumeratePhysicalDevices);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceProperties);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceProperties2);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceFeatures);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceFeatures2);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceFormatProperties);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR)
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkEnumerateDeviceExtensionProperties);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkCreateDevice);
    LICHT_LOAD_RHI_INSTANCE_FUNCTION(vkDestroySurfaceKHR);

    return true;
}

bool vulkan_device_api_load(VkDevice device) {
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkGetDeviceQueue);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDeviceWaitIdle);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateCommandPool);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyCommandPool);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyDevice);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateSwapchainKHR);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroySwapchainKHR);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkGetSwapchainImagesKHR);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateImage);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateImageView);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyImage);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyImageView);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateRenderPass);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyRenderPass);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateFramebuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyFramebuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkGetImageMemoryRequirements);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkAllocateMemory);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkFreeMemory);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkAllocateCommandBuffers);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkFreeCommandBuffers);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkBeginCommandBuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkEndCommandBuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkResetCommandBuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkBindImageMemory);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateSemaphore);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroySemaphore);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateFence);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyFence);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkWaitForFences);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkAcquireNextImageKHR);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkResetFences);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateDescriptorSetLayout);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateDescriptorPool);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyDescriptorPool);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateShaderModule);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyShaderModule);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateSampler);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroySampler);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateBuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyBuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkGetBufferMemoryRequirements);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkBindBufferMemory);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkMapMemory);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkUnmapMemory);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkFlushMappedMemoryRanges);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreatePipelineLayout);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyPipelineLayout);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCreateGraphicsPipelines);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkDestroyPipeline);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdBindPipeline);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkAllocateDescriptorSets);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkFreeDescriptorSets);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkUpdateDescriptorSets);

    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdPipelineBarrier);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdBlitImage);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdCopyBuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdCopyBufferToImage);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdCopyImageToBuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdExecuteCommands);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetViewport);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetScissor);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetFrontFace);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetCullMode);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetStencilTestEnable);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetDepthTestEnable);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetDepthWriteEnable);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetStencilReference);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetStencilOp);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdBeginRendering);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdEndRendering);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetStencilCompareMask);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetStencilWriteMask);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdClearColorImage);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdClearDepthStencilImage);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetPrimitiveTopology);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdPushConstants);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdBindDescriptorSets);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdBindVertexBuffers);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdBindIndexBuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdDraw);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdDrawIndexed);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdBeginRenderPass);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdEndRenderPass);

    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkQueueSubmit);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkQueueWaitIdle);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkQueuePresentKHR);

    return true;
}

}  //namespace licht
