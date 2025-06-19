#include "vulkan_loader.hpp"

#include "licht/core/defines.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_context.hpp"
#include "licht/platform/dynamic_library.hpp"

#define LICHT_LOAD_RHI_CORE_FUNCTION(Name)                                                                                               \
    p_context->rhi.licht_##Name = reinterpret_cast<PFN_##Name>(::licht::DynamicLibraryLoader::load_function(p_context->library, #Name)); \
    if (p_context->rhi.licht_##Name == nullptr) {                                                                                        \
        LLOG_ERROR("[Vulkan]", "Failed to load Vulkan function: " #Name);                                          \
    }

#define LICHT_LOAD_RHI_INSTANCE_FUNCTION(Name)                                                                        \
    p_context->rhi.licht_##Name = (PFN_##Name)p_context->rhi.licht_vkGetInstanceProcAddr(p_context->instance, #Name); \
    if (p_context->rhi.licht_##Name == nullptr) {                                                                     \
        LLOG_ERROR("[Vulkan]", "Failed to load Vulkan instance function: " #Name);              \
    }

#define LICHT_LOAD_RHI_DEVICE_FUNCTION(Name)                                                                                        \
    p_context->rhi.licht_##Name = reinterpret_cast<PFN_##Name>(p_context->rhi.licht_vkGetDeviceProcAddr(p_context->device, #Name)); \
    if (p_context->rhi.licht_##Name == nullptr) {                                                                 \
        LLOG_ERROR("[Vulkan]", "Failed to load Vulkan device function: " #Name);            \
    }                                                                                           

namespace licht {

bool vulkan_library_load(VulkanContext* p_context) {
    LCHECK(p_context)

#if defined(_WIN32)
    p_context->library = DynamicLibraryLoader::load("vulkan-1.dll");
#endif

    return bool(p_context->library);
}

bool vulkan_core_load(VulkanContext* p_context) {
    LCHECK(p_context);

    LICHT_LOAD_RHI_CORE_FUNCTION(vkGetInstanceProcAddr);
    LICHT_LOAD_RHI_CORE_FUNCTION(vkEnumerateInstanceVersion);
    LICHT_LOAD_RHI_CORE_FUNCTION(vkEnumerateInstanceExtensionProperties);
    LICHT_LOAD_RHI_CORE_FUNCTION(vkEnumerateInstanceLayerProperties);
    LICHT_LOAD_RHI_CORE_FUNCTION(vkCreateInstance);

    return true;
}

bool vulkan_instance_load(VulkanContext* p_context) {
    LCHECK(p_context);

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

bool vulkan_device_load(VulkanContext* p_context) {
    LCHECK(p_context);

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
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkGetImageMemoryRequirements);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkAllocateMemory);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkFreeMemory);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkAllocateCommandBuffers);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkFreeCommandBuffers);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkBeginCommandBuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkEndCommandBuffer);
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
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdBindVertexBuffers);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdBindIndexBuffer);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdDraw);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdDrawIndexed);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkCmdBindDescriptorSets);

    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkQueueSubmit);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkQueueWaitIdle);
    LICHT_LOAD_RHI_DEVICE_FUNCTION(vkQueuePresentKHR);

    return true;
}


}  //namespace licht
