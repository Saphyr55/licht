#pragma once

#include "licht/rhi/texture_pool.hpp"

#include "licht/core/memory/allocator.hpp"
#include "licht/core/memory/memory_pool.hpp"
#include "licht/rhi_vulkan/vulkan_texture.hpp"

namespace licht {

class VulkanTexturePool : public RHITexturePool {
public:
    virtual void initialize_pool(Allocator* allocator, size_t block_count) override;

    virtual RHITexture* create_texture(const RHITextureDescription& description) override;

    virtual void destroy_texture(RHITexture* texture) override;

    virtual void dispose() override;

public:
    VulkanTexturePool() = default;

    virtual ~VulkanTexturePool() override {
        dispose();
    }

private:
    void destroy_vulkan_texture(VulkanTexture* vk_texture);

private:
    MemoryPool<VulkanTexture> pool_;
};

}  //namespace licht