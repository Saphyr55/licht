#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"

namespace licht {

struct RHIFramebufferDescription {
    uint32 width;
    uint32 height;
    uint32 layers = 1;
};

class RHIFramebuffer {
public:
    virtual RHIFramebufferDescription get_description() = 0;

public:
    RHIFramebuffer();
    virtual ~RHIFramebuffer() = default;
};

using RHIFramebufferHandle = SharedRef<RHIFramebuffer>;

}  //namespace licht