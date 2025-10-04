#pragma once

#include "licht/core/defines.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/rhi_fowards.hpp"

namespace licht {

struct RHIDescriptorSetInformation {
    uint32 image_count = 0;

    RHIDescriptorSetInformation() = default;
    RHIDescriptorSetInformation(uint32 in_imaga_count)
        : image_count(in_imaga_count) {}

}; 

struct RHIDescriptorSetLayoutBinding {
    uint32 binding = 0;
    uint32 count = 1;
    RHIDescriptorSetType type = RHIDescriptorSetType::Uniform;

    RHIDescriptorSetLayoutBinding() = default;
    RHIDescriptorSetLayoutBinding(uint32 in_binding, uint32 in_count, RHIDescriptorSetType in_type) 
         : type(in_type)
         , binding(in_binding)
         , count(in_count) { }
};

class RHIDescriptorSet {
public:
    virtual void update(RHIBufferRef buffer, size_t binding, size_t offset, size_t range) = 0;

    virtual ~RHIDescriptorSet() = default;
};


class RHIDescriptorPool {
public:
    virtual RHIDescriptorSetRef get_descriptor_set(uint32 index) = 0;

    virtual ~RHIDescriptorPool() = default;
};

}
