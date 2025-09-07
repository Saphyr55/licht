#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"

namespace licht {

class RHIRenderSurface {
public:
    virtual void* get_platform_window() = 0;
};

using RHIRenderSurfaceHandle = SharedRef<RHIRenderSurface>;

}  //namespace licht