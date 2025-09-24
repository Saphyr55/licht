#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/texture.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanTexture : public RHITexture {
public:
    VkImage& get_handle() {
        return handle_;
    }

public:
    RHIVulkanTexture() = default;
    RHIVulkanTexture(VkImage handle)
        : handle_(handle) {
    }
    ~RHIVulkanTexture() = default;

private:
    VkImage handle_= VK_NULL_HANDLE;;
};

class RHIVulkanTextureView : public RHITextureView {
public:
    static RHIVulkanTextureView create(VkImageView view);

public:
    VkImageView& get_handle() {
        return handle_;
    }

public:
    RHIVulkanTextureView() = default;
    RHIVulkanTextureView(VkImageView handle)
        : handle_(handle) {
    }
    ~RHIVulkanTextureView() = default;

private:
    VkImageView handle_= VK_NULL_HANDLE;
};

using RHIVulkanTextureViewRef = SharedRef<RHIVulkanTextureView>;
using RHIVulkanTextureRef = SharedRef<RHIVulkanTexture>;

}  //namespace licht