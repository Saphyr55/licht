#include "vulkan_command_buffer.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_context.hpp"

namespace licht {

void vulkan_command_buffer_init(VulkanContext* p_context) {
    LCHECK(p_context)

    p_context->command_buffers.resize(VulkanContext::MaxFrame);

    VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.commandPool = p_context->command_pool;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = static_cast<uint32>(p_context->command_buffers.size());

    LICHT_VULKAN_CHECK(p_context->api.licht_vkAllocateCommandBuffers(p_context->device, &command_buffer_allocate_info, p_context->command_buffers.data()));
}

void vulkan_command_buffer_begin(VulkanContext* p_context, VkCommandBuffer p_command_buffer) {
    LCHECK(p_context)

    VkCommandBufferBeginInfo command_buffer_begin_info = {};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = 0; // Optional
    command_buffer_begin_info.pInheritanceInfo = nullptr; // Optional

    LICHT_VULKAN_CHECK(p_context->api.licht_vkBeginCommandBuffer(p_command_buffer, &command_buffer_begin_info)); 
    
}

void vulkan_command_buffer_draw(VulkanContext *p_context, VkCommandBuffer p_command_buffer, uint32 p_vertex_count, uint32 p_instance_count, uint32 p_first_vertex, uint32 p_first_instance) {
    LCHECK(p_context)

    p_context->api.licht_vkCmdDraw(p_command_buffer, p_vertex_count, p_instance_count, p_first_vertex, p_first_instance);
}

void vulkan_command_buffer_end(VulkanContext* p_context, VkCommandBuffer p_command_buffer) {
    LCHECK(p_context)

    LICHT_VULKAN_CHECK(p_context->api.licht_vkEndCommandBuffer(p_command_buffer));
}

}