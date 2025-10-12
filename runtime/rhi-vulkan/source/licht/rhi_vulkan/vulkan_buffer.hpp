#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/rhi_types.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class VulkanBuffer : public RHIBuffer {
public:
    virtual RHIBufferUsageFlags get_usage() override;

    virtual RHISharingMode get_sharing_mode() override;

    virtual size_t get_size() override;

    virtual void bind() override;

    virtual void* map(size_t offset = 0, size_t size = 0) override;

    virtual void unmap() override;

    virtual void update(const void* data, size_t size, size_t offset = 0) override;

    VkMemoryRequirements get_memory_requirements();
        
    Array<uint32> get_queue_family_indices();
    
    void initialize(const RHIBufferDescription& description, VkBuffer buffer);
    void initialize(const RHIBufferDescription& description);

    void destroy();

    VkBuffer get_handle();

public:
    VulkanBuffer() = default;
    ~VulkanBuffer() = default;

private:
    RHIBufferDescription description_= {};
    VkDeviceMemory memory_ = VK_NULL_HANDLE;
    VkBuffer buffer_ = VK_NULL_HANDLE;
};

}  //namespace licht