#pragma once

#include "licht/core/defines.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/rhi_fowards.hpp"

namespace licht {

struct RHIShaderResourceBinding {
    uint32 binding = 0;
    uint32 count = 1;
    RHIShaderResourceType type = RHIShaderResourceType::Uniform;
    RHIShaderStage stage = RHIShaderStage::AllGraphics;
};

class RHIShaderResourceGroupLayout {
public:
    virtual ~RHIShaderResourceGroupLayout() = default;

    virtual RHIShaderResourceType get_resource_type(size_t binding) const = 0;

protected:
    RHIShaderResourceGroupLayout() = default;
};

class RHIShaderResourceGroup {
public:
    virtual void set_buffer(RHIBuffer* buffer, size_t binding, size_t offset, size_t range) = 0;

    virtual ~RHIShaderResourceGroup() = default;
};

class RHIShaderResourceGroupPool {
public:
    virtual RHIShaderResourceGroup* allocate_group(RHIShaderResourceGroupLayout* layout) = 0;

    virtual RHIShaderResourceGroup* get_group(size_t pool_index) = 0;

    virtual void deallocate_group(RHIShaderResourceGroup* group) = 0;

    virtual ~RHIShaderResourceGroupPool() = default;
};

}
