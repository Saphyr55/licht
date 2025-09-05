#include "vulkan_command_pool.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

void vulkan_command_pool_init(VulkanContext* p_context) {

}

void vulkan_command_pool_destroy(VulkanContext* p_context) {
    VulkanAPI::lvkDestroyCommandPool(p_context->device->get_handle(), p_context->command_pool, p_context->allocator);
}

}