#pragma once

#include "licht/rhi/rhi_types.hpp"
#include "licht/core/memory/shared_ref.hpp"

namespace licht {

enum class RHIDescriptorSetType {
    Uniform,
    Sampler,
    StorageBuffer,
    StorageImage,
    CombinedImageSampler
};

struct RHIDescriptorSetLayoutBinding {
    uint32 binding = 0;
    uint32 count = 1;
    RHIDescriptorSetType type = RHIDescriptorSetType::Uniform;
};

class RHIDescriptorSetLayout {
public:
    virtual ~RHIDescriptorSetLayout() = default;
};

using RHIDescriptorSetHandle = SharedRef<RHIDescriptorSetLayout>;

}
