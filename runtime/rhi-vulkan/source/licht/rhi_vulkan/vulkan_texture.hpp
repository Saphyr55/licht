#pragma once

#include "licht/rhi/texture.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class VulkanTexture : public RHITexture {
public:
    VkMemoryRequirements get_memory_requirements();

    void initialize(const RHITextureDescription& description);

    void destroy();

    virtual void bind() override;

    VkImage& get_handle() {
        return handle_;
    }

    const VkImage& get_handle() const {
        return handle_;
    }

public:
    VulkanTexture() = default;
    virtual ~VulkanTexture() override = default;

private:
    VkImage handle_;
    VkDeviceMemory memory_;
};

}  //namespace licht