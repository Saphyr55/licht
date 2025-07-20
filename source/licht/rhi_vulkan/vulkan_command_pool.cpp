#include "vulkan_command_pool.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

void vulkan_command_pool_init(VulkanContext* p_context) {

    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_create_info.queueFamilyIndex = p_context->physical_device.graphics_queue_index;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateCommandPool(p_context->device, &command_pool_create_info, p_context->allocator, &p_context->command_pool));
}

void vulkan_command_pool_destroy(VulkanContext* p_context) {
    VulkanAPI::lvkDestroyCommandPool(p_context->device, p_context->command_pool, p_context->allocator);
}

}