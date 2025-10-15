#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

/**
 * @brief Defines a single resource binding slot within a Shader Resource Group Layout.
 */
struct RHIShaderResourceBinding {
    uint32 binding = 0;
    uint32 count = 1;
    RHIShaderResourceType type = RHIShaderResourceType::Uniform;
    RHIShaderStage stage = RHIShaderStage::AllGraphics;

    RHIShaderResourceBinding() = default;
    RHIShaderResourceBinding(uint32 in_binding,
                             RHIShaderResourceType in_type,
                             RHIShaderStage in_stage = RHIShaderStage::AllGraphics,
                             uint32 in_count = 1)
        : binding(in_binding)
        , count(in_count)
        , type(in_type)
        , stage(in_stage) {}
};

/**
 * @brief Data structure to update a Buffer (Uniform or Storage) resource in a Shader Resource Group.
 */
struct RHIWriteBufferResource {
    RHIBuffer* buffer = nullptr;
    uint32 binding = 0;
    size_t offset = 0;
    size_t range = 0;

    RHIWriteBufferResource() = default;
    RHIWriteBufferResource(uint32 in_binding,
                           RHIBuffer* in_buffer,
                           size_t in_offset = 0,
                           size_t in_range = 0)
        : buffer(in_buffer)
        , binding(in_binding)
        , offset(in_offset)
        , range(in_range) {}
};

/**
 * @brief Data structure to update a Sampler resource (independent Sampler binding).
 */
struct RHIWriteSamplerResource {
    RHISampler* sampler = nullptr;
    uint32 binding = 0;

    RHIWriteSamplerResource() = default;
    RHIWriteSamplerResource(uint32 in_binding, RHISampler* in_sampler)
        : sampler(in_sampler)
        , binding(in_binding) {}
};

/**
 * @brief Data structure to update a Combined Image Sampler resource (Texture + Sampler).
 */
struct RHIWriteTextureSamplerResource {
    RHITextureView* texture_view = nullptr;
    RHISampler* sampler = nullptr;
    uint32 binding = 0;

    RHIWriteTextureSamplerResource() = default;
    RHIWriteTextureSamplerResource(uint32 in_binding,
                                   RHITextureView* in_texture_view,
                                   RHISampler* in_sampler)
        : texture_view(in_texture_view)
        , sampler(in_sampler)
        , binding(in_binding) {}
};

/**
 * @brief Data structure to update a Read-Only Texture (Sampled Image).
 * Used when the sampler is immutable or bound separately.
 */
struct RHIWriteTextureResource {
    RHITextureView* texture_view = nullptr;
    uint32 binding = 0;

    RHIWriteTextureResource() = default;
    RHIWriteTextureResource(uint32 in_binding, RHITextureView* in_texture_view)
        : texture_view(in_texture_view)
        , binding(in_binding) {}
};

/**
 * @brief Base class representing the immutable definition of a Shader Resource Group (VkDescriptorSetLayout).
 */
class RHIShaderResourceGroupLayout {
public:
    virtual ~RHIShaderResourceGroupLayout() = default;

    /**
     * @brief Gets the RHI type of the resource bound to a specific index.
     * @param binding The binding index within this layout.
     * @return The resource type (e.g., Uniform, Sampler).
     */
    virtual RHIShaderResourceType get_resource_type(size_t binding) const = 0;

    virtual const Array<RHIShaderResourceBinding>& get_bindings() const = 0;

protected:
    RHIShaderResourceGroupLayout() = default;
};

/**
 * @brief Base class representing a single instance of a Shader Resource Group (VkDescriptorSet).
 */
class RHIShaderResourceGroup {
public:
    /**
     * @brief Writes an RHI Buffer (Uniform or Storage) to the specified binding.
     */
    virtual void set_buffer(const RHIWriteBufferResource& resource) = 0;

    /**
     * @brief Writes a separate RHI Sampler to the specified binding.
     */
    virtual void set_sampler(const RHIWriteSamplerResource& resource) = 0;

    /**
     * @brief Writes a Combined Image Sampler (Texture and Sampler) to the specified binding.
     * Use this for textures that are sampled in the shader.
     */
    virtual void set_texture_sampler(const RHIWriteTextureSamplerResource& resource) = 0;

    /**
     * @brief Writes a Storage Image or a Read-Only Texture View to the specified binding.
     * Use this when the sampler is not part of the binding.
     */
    virtual void set_texture(const RHIWriteTextureResource& resource) = 0;

    /**
     * @brief Finalizes the group if internal buffering or compilation is required (less common in Vulkan).
     * This method is often a no-op but included for RHI flexibility.
     */
    virtual void compile() = 0;

    virtual ~RHIShaderResourceGroup() = default;
};

/**
 * @brief Base class for managing the allocation and deallocation of Shader Resource Groups (VkDescriptorPool).
 */
class RHIShaderResourceGroupPool {
public:
    /**
     * @brief Allocates a new Shader Resource Group based on the provided layout.
     */
    virtual RHIShaderResourceGroup* allocate_group(RHIShaderResourceGroupLayout* layout) = 0;

    /**
     * @brief Returns a Shader Resource Group to the pool for reuse.
     */
    virtual void deallocate_group(RHIShaderResourceGroup* group) = 0;

    virtual void dispose() = 0;

    /**
     * @brief Retrieves a Shader Resource Group by its internal pool index (e.g., for pre-allocated pools).
     */
    virtual RHIShaderResourceGroup* get_group(size_t pool_index) = 0;

    virtual size_t get_count() = 0;

    virtual size_t get_max_count() = 0;

    virtual ~RHIShaderResourceGroupPool() = default;
};

}  // namespace licht