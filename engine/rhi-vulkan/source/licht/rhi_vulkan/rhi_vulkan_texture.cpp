#include "licht/rhi_vulkan/rhi_vulkan_texture.hpp"

namespace licht {

RHIVulkanTextureView RHIVulkanTextureView::create(VkImageView view) {
    return RHIVulkanTextureView(view);
}

}