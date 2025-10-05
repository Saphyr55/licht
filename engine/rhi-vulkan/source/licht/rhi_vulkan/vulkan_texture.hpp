#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi/texture.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class VulkanTexture : public RHITexture {
public:
    VkMemoryRequirements get_memory_requirements();

    void initialize();

    void destroy();

    virtual void bind() override;

    virtual const RHITextureDescription& get_description() const override {
        return description_;
    }

    VkImage& get_handle() {
        return handle_;
    }

    const VkImage& get_handle() const {
        return handle_;
    }

public:
    VulkanTexture(VulkanContext& context,
                  const RHITextureDescription& description,
                  VkImageType type,
                  const Array<uint32>& queue_family_indices)
        : context_(context)
        , description_(description)
        , type_(type)
        , queue_family_indices_(queue_family_indices)
        , handle_(VK_NULL_HANDLE)
        , memory_(VK_NULL_HANDLE) {
    }

    ~VulkanTexture() = default;

private:
    VulkanContext& context_;
    RHITextureDescription description_;
    VkImageType type_;
    VkDeviceMemory memory_;
    VkImage handle_;
    Array<uint32> queue_family_indices_;
};

class VulkanTextureView : public RHITextureView {
public:
    VkImageView& get_handle() {
        return handle_;
    }

    void initialize();

    void destroy();

public:
    VulkanTextureView()
        : handle_(VK_NULL_HANDLE) {
    }

    ~VulkanTextureView() = default;

private:
    VkImageView handle_;
};

}  //namespace licht