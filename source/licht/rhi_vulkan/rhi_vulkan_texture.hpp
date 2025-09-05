#pragma once

#include "licht/rhi/texture.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

class RHIVulkanTextureView : public RHITextureView {
public:
    VkImageView& get_handle();

private:
    VkImageView handle_;
};

}