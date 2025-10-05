#pragma once

#include "licht/core/defines.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/rhi_fowards.hpp"

namespace licht {

struct RHIShaderResourceBinding {
    uint32 binding = 0;
    uint32 count = 1;
    RHIShaderResourceBindingType type = RHIShaderResourceBindingType::Uniform;

    RHIShaderResourceBinding() = default;
    RHIShaderResourceBinding(uint32 in_binding, uint32 in_count, RHIShaderResourceBindingType in_type) 
         : type(in_type)
         , binding(in_binding)
         , count(in_count) { }
};

class RHIShaderResource {
public:
    virtual void update(RHIBuffer* buffer, size_t binding, size_t offset, size_t range) = 0;

    virtual ~RHIShaderResource() = default;
};


class RHIShaderResourcePool {
public:
    virtual RHIShaderResource* get_shader_resource(uint32 index) = 0;

    virtual ~RHIShaderResourcePool() = default;
};

}
