#pragma once

#include <vulkan/vulkan_core.h>
#include "licht/core/defines.hpp"

namespace licht {

struct VulkanContext;

void vulkan_command_buffer_init(VulkanContext* p_context);

void vulkan_command_buffer_begin(VulkanContext* p_context, VkCommandBuffer p_command_buffer);

void vulkan_command_buffer_draw(VulkanContext *p_context, VkCommandBuffer p_command_buffer, uint32 p_vertex_count, uint32 p_instance_count, uint32 p_first_vertex, uint32 p_first_instance);

void vulkan_command_buffer_end(VulkanContext* p_context, VkCommandBuffer p_command_buffer);

}