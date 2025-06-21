#pragma once

#include <vulkan/vulkan_core.h>

#include "licht/core/defines.hpp"

namespace licht {

struct VulkanContext;

void vulkan_render_pass_init(VulkanContext* p_context);

void vulkan_render_pass_begin(VulkanContext* p_context, VkCommandBuffer p_command_buffer, uint32 p_image_index);

void vulkan_render_pass_end(VulkanContext* p_context, VkCommandBuffer p_command_buffer);

void vulkan_render_pass_destroy(VulkanContext* p_context);

}