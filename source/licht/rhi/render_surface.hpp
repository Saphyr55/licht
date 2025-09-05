#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"

namespace licht {

class RHIRenderSurface {
public:
    virtual uint32 get_width() = 0;

    virtual uint32 get_height() = 0;

    virtual void* get_platform_window() = 0;
};

using RHIRenderSurfaceHandle = SharedRef<RHIRenderSurface>;

}  //namespace licht