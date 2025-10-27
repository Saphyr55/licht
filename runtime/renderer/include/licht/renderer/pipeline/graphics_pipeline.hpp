#pragma once

#include "licht/rhi/rhi_forwards.hpp"

namespace licht {

class GraphicsPipeline {
public:


protected:
    RHIGraphicsPipeline* handle_;
    RHIRenderPass* render_pass_ = nullptr;

};

}