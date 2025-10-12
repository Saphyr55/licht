#pragma once

#include "buffer_pool.hpp"
#include "licht/core/containers/array_view.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/buffer_pool.hpp"
#include "licht/rhi/rhi_exports.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/texture.hpp"

namespace licht {

struct LICHT_RHI_API RHIStagingBufferContext {
    RHIBufferUsageFlags usage;
    size_t size;       // Size in bytes;
    const void* data;  // Data to transfer to the device.

    RHIStagingBufferContext(RHIBufferUsageFlags in_usage, size_t in_size, void* in_data)
        : usage(in_usage)
        , size(in_size)
        , data(in_data) {}

    template <typename T>
    RHIStagingBufferContext(RHIBufferUsageFlags usage, const ArrayView<T>& data)
        : usage(usage)
        , size(data.size() * sizeof(T))
        , data(data.data()) {
    }

    ~RHIStagingBufferContext() = default;
};

class LICHT_RHI_API RHIDeviceMemoryUploader {
public:
    RHIBuffer* send_buffer(const RHIStagingBufferContext& context);

    RHITexture* send_texture(const RHIStagingBufferContext& context, RHITextureDescription& description);

    void upload();

    RHIDeviceMemoryUploader(RHIDeviceRef device, RHIBufferPoolRef buffer_pool, size_t capacity = 8);
    
private:
    RHIBufferDescription create_staging_buffer_description(const RHIStagingBufferContext& context);
    RHIBufferDescription create_buffer_description(const RHIStagingBufferContext& context);

private:
    struct BufferEntry {
        RHIBuffer* staging;
        RHIBuffer* buffer;
        size_t size;
    };

    struct TextureEntry {
        RHIBuffer* staging;
        RHITexture* texture;
        size_t size;
    };

    RHIBufferPoolRef staging_buffer_pool_;
    RHIBufferPoolRef buffer_pool_;
    RHIDeviceRef device_;
    Array<BufferEntry> buffer_entries_;
    Array<TextureEntry> texture_entries_;
};

}