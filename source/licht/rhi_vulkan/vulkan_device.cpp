#include "licht/rhi_vulkan/vulkan_device.hpp"

namespace licht {

void VulkanDevice::initialize() {
    physical_device_.select_physical_device();
    logical_device_.initialize();
}

void VulkanDevice::destroy() {

}

}  //namespace licht
