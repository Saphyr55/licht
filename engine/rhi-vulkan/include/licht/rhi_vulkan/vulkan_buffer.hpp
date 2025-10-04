#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanBuffer : public RHIBuffer {
public:
    virtual RHIBufferUsage get_usage() override;

    virtual RHIAccessMode get_access_mode() override;
    
    virtual size_t get_size() override;

    virtual void bind() override;

    virtual void* map(size_t offset = 0, size_t size = 0) override;

    virtual void unmap() override;

    virtual void update(const void* data, size_t size, size_t offset = 0) override;

    VkMemoryRequirements get_memory_requirements();
    
    VkMemoryPropertyFlags get_memory_property_flags();
    
    Array<uint32> get_queue_family_indices();
    
    void initialize();

    void destroy();

    VkBuffer get_handle();

public:
    RHIVulkanBuffer(VulkanContext& context, RHIBufferDescription description);
    ~RHIVulkanBuffer() = default;

private:
    VulkanContext& context_;
    RHIBufferDescription description_;
    VkDeviceMemory memory_;
    VkBuffer buffer_;
};

using RHIVulkanBufferRef = SharedRef<RHIVulkanBuffer>;

}  //namespace licht