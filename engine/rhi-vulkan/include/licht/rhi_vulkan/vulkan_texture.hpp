#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanTexture : public RHITexture {
public:
    static SharedRef<RHIVulkanTexture> create(VulkanContext& context,
                                              const RHITextureDescription& description,
                                              VkImageType type,
                                              VkImageUsageFlags usage,
                                              VkSharingMode sharing_mode,
                                              const Array<uint32> queue_family_indices);
    static void destroy(VulkanContext& context, RHIVulkanTexture& texture);

    static VkMemoryRequirements get_memory_requirements(VulkanContext& context, VkImage image);

    virtual void bind() override;

    VkImage& get_handle() {
        return handle_;
    }

public:
    RHIVulkanTexture(VulkanContext& context, VkImage handle, VkDeviceMemory memory)
        : context_(context), handle_(handle), memory_(memory) {
    }

    ~RHIVulkanTexture() = default;

private:
    VulkanContext& context_;
    VkDeviceMemory memory_ = VK_NULL_HANDLE;
    VkImage handle_ = VK_NULL_HANDLE;
};

class RHIVulkanTextureView : public RHITextureView {
public:

public:
    VkImageView& get_handle() {
        return handle_;
    }

public:
    RHIVulkanTextureView(VkImageView handle)
        : handle_(handle) {
    }

    ~RHIVulkanTextureView() = default;

private:
    VkImageView handle_ = VK_NULL_HANDLE;
};

using RHIVulkanTextureViewRef = SharedRef<RHIVulkanTextureView>;
using RHIVulkanTextureRef = SharedRef<RHIVulkanTexture>;

}  //namespace licht