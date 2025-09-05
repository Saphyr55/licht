#pragma once

#include "licht/core/collection/array.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class VulkanRHI {
public:
    static Array<VkExtensionProperties> get_instance_extension_properties();
};

}  //namespace licht