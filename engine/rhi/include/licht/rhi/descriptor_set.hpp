#pragma once

#include "licht/rhi/rhi_types.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/containers/array_view.hpp"
#include "licht/rhi/buffer.hpp"

namespace licht {

struct RHIDescriptorSetInformation {
    uint32 image_count;

    RHIDescriptorSetInformation(uint32 in_imaga_count)
        : image_count(in_imaga_count) {}
}; 

struct RHIDescriptorSetLayoutBinding {
    uint32 binding = 0;
    uint32 count = 1;
    RHIDescriptorSetType type = RHIDescriptorSetType::Uniform;
};

class RHIDescriptorSet {
public:
    virtual void update(RHIBufferHandle buffer, size_t binding, size_t offset, size_t range) = 0;

    virtual ~RHIDescriptorSet() = default;
};

using RHIDescriptorSetRef = SharedRef<RHIDescriptorSet>;

class RHIDescriptorPool {
public:
    virtual RHIDescriptorSetRef get_descriptor_set(uint32 index) = 0;

    virtual ~RHIDescriptorPool() = default;
};

using RHIDescriptorPoolRef = SharedRef<RHIDescriptorPool>;

}
