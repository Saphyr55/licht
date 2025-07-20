#pragma once

#include <vulkan/vulkan_core.h>

#include "licht/core/string/string_ref.hpp"

namespace licht {

struct VulkanContext;

// TODO: Make it not global.
static const Array<StringRef> g_physical_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

void vulkan_logical_device_init(VulkanContext* p_context);

void vulkan_queues_init(VulkanContext* p_context);

void vulkan_logical_device_destroy(VulkanContext* p_context);

}  //namespace licht