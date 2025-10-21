#pragma once

#include "licht/core/string/string.hpp"
#include "licht/renderer/pipeline/graphics_pipeline_config.hpp"
#include "licht/rhi/shader_resource.hpp"

namespace licht {

class MaterialTemplate {
public:

private:
    String name_;
    GraphicsPipelineConfig pipeline_config_;
    Array<RHIShaderResourceGroupLayout*> layouts_; // In order.
};

}