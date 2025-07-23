#include "vulkan_logical_device.hpp"

#include <vulkan/vulkan_core.h>

#include "licht/core/collection/array.hpp"
#include "licht/core/collection/hash_map.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/allocator.hpp"
#include "licht/core/string/format.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"

namespace licht {

VulkanLogicalDevice::VulkanLogicalDevice(VulkanDevice& p_device, VulkanPhysicalDevice& p_physical_device)
    : device_(p_device)
    , physical_device_(p_physical_device) {
}

void VulkanLogicalDevice::initialize() {

}

void VulkanLogicalDevice::destroy() {

}

}  //namespace licht
