#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanTexture : public RHITexture {
public:
    VkMemoryRequirements get_memory_requirements();

    void initialize();

    void destroy();

    virtual void bind() override;

    VkImage& get_handle() {
        return handle_;
    }

public:
    RHIVulkanTexture(VulkanContext& context,
                     const RHITextureDescription& description,
                     VkImageType type,
                     VkImageUsageFlags usage,
                     VkSharingMode sharing_mode,
                     const Array<uint32>& queue_family_indices)
        : context_(context)
        , description_(description)
        , type_(type)
        , usage_(usage)
        , sharing_mode_(sharing_mode)
        , queue_family_indices_(queue_family_indices)
        , handle_(VK_NULL_HANDLE)
        , memory_(VK_NULL_HANDLE) {
    }

    ~RHIVulkanTexture() = default;

private:
    VulkanContext& context_;
    RHITextureDescription description_;
    VkImageType type_;
    VkImageUsageFlags usage_;
    VkSharingMode sharing_mode_;
    VkDeviceMemory memory_;
    VkImage handle_;
    Array<uint32> queue_family_indices_;
};

class RHIVulkanTextureView : public RHITextureView {
public:
    VkImageView& get_handle() {
        return handle_;
    }

    void initialize();

    void destroy();

public:
    RHIVulkanTextureView()
        : handle_(VK_NULL_HANDLE) {
    }

    ~RHIVulkanTextureView() = default;

private:
    VkImageView handle_;
};

using RHIVulkanTextureViewRef = SharedRef<RHIVulkanTextureView>;
using RHIVulkanTextureRef = SharedRef<RHIVulkanTexture>;

}  //namespace licht