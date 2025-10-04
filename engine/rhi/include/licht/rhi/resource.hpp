#pragma once

#include "licht/core/defines.hpp"
#include "licht/rhi/rhi_fowards.hpp"

namespace licht {

class RHIResource {
public:
    virtual ~RHIResource() = default;
};

}  //namespace licht