#pragma once

#include "licht/core/platform/dynamic_library.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_exports.hpp"

#include <vulkan/vulkan_core.h>

#define LICHT_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) l##Name

#define LICHT_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name) PFN_##Name

#define LICHT_DEFINE_RHI_FUNCTION(Name) \
    static LICHT_RHI_VULKAN_API LICHT_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name) LICHT_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name);

#define LICHT_DEFINE_RHI_FUNCTION_IMPL(Name) \
    LICHT_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name) VulkanAPI::LICHT_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) = nullptr;

namespace licht {

struct VulkanContext;

class VulkanAPI {
public:
    // Core Vulkan functions.
    LICHT_DEFINE_RHI_FUNCTION(vkGetInstanceProcAddr);
    LICHT_DEFINE_RHI_FUNCTION(vkEnumerateInstanceVersion);
    LICHT_DEFINE_RHI_FUNCTION(vkEnumerateInstanceExtensionProperties);
    LICHT_DEFINE_RHI_FUNCTION(vkEnumerateInstanceLayerProperties);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateInstance);

    // Instance functions.
    LICHT_DEFINE_RHI_FUNCTION(vkGetDeviceProcAddr);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyInstance);
    LICHT_DEFINE_RHI_FUNCTION(vkEnumeratePhysicalDevices);
    LICHT_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceProperties);
    LICHT_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceProperties2);
    LICHT_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceFeatures);
    LICHT_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceFeatures2);
    LICHT_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
    LICHT_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
    LICHT_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceFormatProperties);
    LICHT_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    LICHT_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
    LICHT_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
    LICHT_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR)
    LICHT_DEFINE_RHI_FUNCTION(vkEnumerateDeviceExtensionProperties);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateDevice);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroySurfaceKHR);

    // Device functions.
    LICHT_DEFINE_RHI_FUNCTION(vkGetDeviceQueue);
    LICHT_DEFINE_RHI_FUNCTION(vkDeviceWaitIdle);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateCommandPool);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyCommandPool);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyDevice);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateSwapchainKHR);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroySwapchainKHR);
    LICHT_DEFINE_RHI_FUNCTION(vkGetSwapchainImagesKHR);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateImage);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateImageView);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyImage);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyImageView);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateRenderPass);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyRenderPass);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateFramebuffer);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyFramebuffer);
    LICHT_DEFINE_RHI_FUNCTION(vkGetImageMemoryRequirements);
    LICHT_DEFINE_RHI_FUNCTION(vkAllocateMemory);
    LICHT_DEFINE_RHI_FUNCTION(vkFreeMemory);
    LICHT_DEFINE_RHI_FUNCTION(vkAllocateCommandBuffers);
    LICHT_DEFINE_RHI_FUNCTION(vkFreeCommandBuffers);
    LICHT_DEFINE_RHI_FUNCTION(vkBeginCommandBuffer);
    LICHT_DEFINE_RHI_FUNCTION(vkEndCommandBuffer);
    LICHT_DEFINE_RHI_FUNCTION(vkResetCommandBuffer);
    LICHT_DEFINE_RHI_FUNCTION(vkBindImageMemory);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateSemaphore);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroySemaphore);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateFence);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyFence);
    LICHT_DEFINE_RHI_FUNCTION(vkWaitForFences);
    LICHT_DEFINE_RHI_FUNCTION(vkAcquireNextImageKHR);
    LICHT_DEFINE_RHI_FUNCTION(vkResetFences);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateDescriptorSetLayout);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyDescriptorSetLayout);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateDescriptorPool);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyDescriptorPool);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateShaderModule);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyShaderModule);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateSampler);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroySampler);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateBuffer);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyBuffer);
    LICHT_DEFINE_RHI_FUNCTION(vkGetBufferMemoryRequirements);
    LICHT_DEFINE_RHI_FUNCTION(vkBindBufferMemory);
    LICHT_DEFINE_RHI_FUNCTION(vkMapMemory);
    LICHT_DEFINE_RHI_FUNCTION(vkUnmapMemory);
    LICHT_DEFINE_RHI_FUNCTION(vkFlushMappedMemoryRanges);
    LICHT_DEFINE_RHI_FUNCTION(vkCreatePipelineLayout);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyPipelineLayout);
    LICHT_DEFINE_RHI_FUNCTION(vkCreateGraphicsPipelines);
    LICHT_DEFINE_RHI_FUNCTION(vkDestroyPipeline);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdBindPipeline);
    LICHT_DEFINE_RHI_FUNCTION(vkAllocateDescriptorSets);
    LICHT_DEFINE_RHI_FUNCTION(vkFreeDescriptorSets);
    LICHT_DEFINE_RHI_FUNCTION(vkUpdateDescriptorSets);

    LICHT_DEFINE_RHI_FUNCTION(vkCmdPipelineBarrier);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdBlitImage);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdCopyBuffer);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdCopyBufferToImage);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdCopyImageToBuffer);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdExecuteCommands);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetViewport);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetScissor);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetFrontFace);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetCullMode);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetStencilTestEnable);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetDepthTestEnable);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetDepthWriteEnable);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetStencilReference);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetStencilOp);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdBeginRendering);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdEndRendering);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetStencilCompareMask);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetStencilWriteMask);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdClearColorImage);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdClearDepthStencilImage);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdSetPrimitiveTopology);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdPushConstants);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdBindDescriptorSets);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdBindVertexBuffers);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdBindIndexBuffer);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdDraw);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdDrawIndexed);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdBeginRenderPass);
    LICHT_DEFINE_RHI_FUNCTION(vkCmdEndRenderPass);

    LICHT_DEFINE_RHI_FUNCTION(vkQueueSubmit);
    LICHT_DEFINE_RHI_FUNCTION(vkQueueWaitIdle);
    LICHT_DEFINE_RHI_FUNCTION(vkQueuePresentKHR);
};

SharedRef<DynamicLibrary> vulkan_library_load();

bool vulkan_core_api_load(SharedRef<DynamicLibrary> library);

bool vulkan_instance_api_load(VkInstance instance);

bool vulkan_device_api_load(VkDevice device);

}
