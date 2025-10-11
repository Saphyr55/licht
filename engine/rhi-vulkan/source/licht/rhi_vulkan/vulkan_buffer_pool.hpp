#pragma once

#include "licht/core/memory/memory_pool.hpp"
#include "licht/rhi/buffer_pool.hpp"
#include "licht/rhi_vulkan/vulkan_buffer.hpp"

namespace licht {

class VulkanBufferPool : public RHIBufferPool {
public:
    virtual void initialize_pool(Allocator* allocator, size_t block_count) override;

    virtual RHIBuffer* create_buffer(const RHIBufferDescription& description) override;

    virtual void destroy_buffer(RHIBuffer* buffer) override;

    VulkanBufferPool() = default;
    virtual ~VulkanBufferPool() override = default;

private:
    MemoryPool<VulkanBuffer> pool_;
};

}