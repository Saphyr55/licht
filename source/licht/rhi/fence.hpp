#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"

namespace licht {

class RHIFence {
public:    
    virtual ~RHIFence() = default;
};

using RHIFenceHandle = SharedRef<RHIFence>;

}  //namespace licht