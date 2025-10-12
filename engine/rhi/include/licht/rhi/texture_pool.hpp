#pragma once

#include "licht/rhi/rhi_exports.hpp"

namespace licht {

class Allocator;
class RHITexture;
struct RHITextureDescription;

class LICHT_RHI_API RHITexturePool {
public:
    virtual void initialize_pool(Allocator* allocator, size_t block_count) = 0;

    virtual RHITexture* create_texture(const RHITextureDescription& description) = 0;

    virtual void destroy_texture(RHITexture* texture) = 0;

    virtual void dispose() = 0;

    virtual ~RHITexturePool() = default;
};

}  //namespace licht