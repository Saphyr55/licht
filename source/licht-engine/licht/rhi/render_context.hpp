#pragma once

#include "licht/rhi/device.hpp"

namespace licht {

enum class RHIGraphicsAPI {
    Vulkan
};

class RHIRenderContext {
public:
    static RHIRenderContext* create(RHIGraphicsAPI graphics_api);

public:
    virtual RHIDevice& get_device() = 0; 

private:
};

}
