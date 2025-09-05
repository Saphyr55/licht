#pragma once

#include "licht/core/memory/shared_ref.hpp"

namespace licht {

class RHISwapchain {
public:
    virtual void present() = 0;

    virtual uint32 get_width() = 0;

    virtual uint32 get_height() = 0;
};

using RHISwapchainHandle = SharedRef<RHISwapchain>;

}