#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

struct VulkanContext;

class VulkanShaderModule {
public:
    void set_code(const Array<uint8>& p_code) { code_ = p_code; }

    void initialize(VulkanContext& p_context);

    void destroy(VulkanContext& p_context);

    VkShaderModule handle();

public:
    VulkanShaderModule() = default;
    VulkanShaderModule(const Array<uint8>& p_code);

private:
    VkShaderModule handle_ = VK_NULL_HANDLE;
    Array<uint8> code_;
};

}  //namespace licht