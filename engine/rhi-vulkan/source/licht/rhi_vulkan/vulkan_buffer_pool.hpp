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

    virtual void dispose() override;

public:
    VulkanBufferPool() = default;

    virtual ~VulkanBufferPool() override {
        dispose();
    }

private:    
    void destroy_vulkan_buffer(VulkanBuffer* vulkan_buffer);

private:
    MemoryPool<VulkanBuffer> pool_;
};

}