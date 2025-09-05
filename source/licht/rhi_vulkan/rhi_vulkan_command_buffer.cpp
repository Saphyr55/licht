#include "rhi_vulkan_command_buffer.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

void vulkan_command_buffer_begin(VulkanContext* p_context, VkCommandBuffer p_command_buffer) {
    LCHECK(p_context)

    VkCommandBufferBeginInfo command_buffer_begin_info = {};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = 0;                   // Optional
    command_buffer_begin_info.pInheritanceInfo = nullptr;  // Optional

    LICHT_VULKAN_CHECK(VulkanAPI::lvkBeginCommandBuffer(p_command_buffer, &command_buffer_begin_info));
}

void vulkan_command_buffer_draw(VulkanContext* p_context, VkCommandBuffer p_command_buffer, uint32 p_vertex_count, uint32 p_instance_count, uint32 p_first_vertex, uint32 p_first_instance) {
    LCHECK(p_context)

    VulkanAPI::lvkCmdDraw(p_command_buffer, p_vertex_count, p_instance_count, p_first_vertex, p_first_instance);
}

void vulkan_command_buffer_end(VulkanContext* p_context, VkCommandBuffer p_command_buffer) {
    LCHECK(p_context)

    LICHT_VULKAN_CHECK(VulkanAPI::lvkEndCommandBuffer(p_command_buffer));
}

const VkCommandPool VulkanCommandBufferRegistry::get_command_pool() const {
    return command_pool_;
}

const Array<VkCommandBuffer>& VulkanCommandBufferRegistry::get_command_buffers() const {
    return command_buffers_;
}

VulkanCommandBufferRegistry::VulkanCommandBufferRegistry(VulkanContext& context, uint32 frame_count)
    : context_(context) {
    VkDevice device = context_.device->get_handle();

    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_create_info.queueFamilyIndex = context_.device->get_info().graphics_queue_index;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateCommandPool(device, &command_pool_create_info, context_.allocator, &command_pool_));

    command_buffers_.resize(frame_count);

    VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.commandPool = command_pool_;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = frame_count;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkAllocateCommandBuffers(device, &command_buffer_allocate_info, command_buffers_.data()));
}

}  //namespace licht