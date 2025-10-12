#pragma once

#include "licht/rhi/texture_view.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class VulkanTextureView : public RHITextureView {
public:
    VkImageView& get_handle() {
        return handle_;
    }

    void initialize(const RHITextureViewDescription& description, VkImageView image_view);
    void initialize(const RHITextureViewDescription& description);

    void destroy();

public:
    VulkanTextureView()
        : handle_(VK_NULL_HANDLE) {
    }

    ~VulkanTextureView() = default;

private:
    VkImageView handle_;
};

}
