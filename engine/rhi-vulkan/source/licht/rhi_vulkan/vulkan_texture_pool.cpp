#include "licht/rhi_vulkan/vulkan_texture_pool.hpp"
#include "vulkan_texture_pool.hpp"

namespace licht {

void VulkanTexturePool::initialize_pool(Allocator* allocator, size_t block_count) {
    pool_.initialize_pool(allocator, block_count);
}

RHITexture* VulkanTexturePool::create_texture(const RHITextureDescription& description) {
    VulkanTexture* vk_texture = pool_.new_resource();
    vk_texture->initialize(description);
    return vk_texture;
}

void VulkanTexturePool::destroy_texture(RHITexture* texture) {
    destroy_vulkan_texture(static_cast<VulkanTexture*>(texture));
}

void VulkanTexturePool::dispose() {
    pool_.for_each([this](VulkanTexture* texture) -> void {
        destroy_vulkan_texture(texture);
    });
    pool_.dispose();
}

void VulkanTexturePool::destroy_vulkan_texture(VulkanTexture* vulkan_texture) {
    vulkan_texture->destroy();
    pool_.destroy_resource(vulkan_texture);
}

}  //namespace licht