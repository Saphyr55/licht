#include "licht/rhi_vulkan/vulkan_sampler.hpp"
#include "licht/rhi/sampler.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan_context.hpp"

namespace licht {

void VulkanSampler::initialize(const RHISamplerDescription& description) {
    description_ = description;

    VulkanContext& context = vulkan_context_get();

    VkSamplerCreateInfo sampler_info_create_info = {};
    sampler_info_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info_create_info.magFilter = vulkan_filter_get(description.mag_filter);
    sampler_info_create_info.minFilter = vulkan_filter_get(description.min_filter);
    sampler_info_create_info.addressModeU = vulkan_address_mode_get(description.address_mode_u);
    sampler_info_create_info.addressModeV = vulkan_address_mode_get(description.address_mode_v);
    sampler_info_create_info.addressModeW = vulkan_address_mode_get(description.address_mode_w);
    sampler_info_create_info.anisotropyEnable = context.physical_device_info.features.samplerAnisotropy;
    sampler_info_create_info.maxAnisotropy = 16.0f;
    sampler_info_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info_create_info.unnormalizedCoordinates = static_cast<VkBool32>(description.normalize);
    sampler_info_create_info.compareEnable = VK_FALSE;
    sampler_info_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info_create_info.mipLodBias = 0.0f;
    sampler_info_create_info.minLod = 0.0f;
    sampler_info_create_info.maxLod = 0.0f;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateSampler(
        context.device,
        &sampler_info_create_info,
        context.allocator,
        &handle_));
}

void VulkanSampler::destroy() {
    if (handle_ != VK_NULL_HANDLE) {
        VulkanAPI::lvkDestroySampler(
            vulkan_context_get().device,
            handle_,
            vulkan_context_get().allocator);
        handle_ = VK_NULL_HANDLE;
    }
}

VulkanSampler::~VulkanSampler() {
}

}  //namespace licht