#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/rhi_exports.hpp"

namespace licht {

class LICHT_RHI_API RHIBufferPool {
public:
    virtual void initialize_pool(Allocator* allocator, size_t block_count) = 0;

    virtual RHIBuffer* create_buffer(const RHIBufferDescription& description) = 0;

    virtual void destroy_buffer(RHIBuffer* buffer) = 0;

    virtual void dispose() = 0;

    virtual ~RHIBufferPool() = default;
};

using RHIBufferPoolRef = SharedRef<RHIBufferPool>;

}  //namespace licht