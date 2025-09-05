#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/resource.hpp"

namespace licht {

class RHIBuffer : public RHIResource {
public:
};

using RHIBufferHandle = SharedRef<RHIBuffer>;

}