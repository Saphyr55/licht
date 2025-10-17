#pragma once

#include "licht/renderer/pipeline/graphics_pipeline_config.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/renderer/renderer_exports.hpp"
#include "licht/rhi/graphics_pipeline.hpp"

namespace licht {

class RHIDevice;

class LICHT_RENDERER_API PipelineCompiler {
public:
    SharedRef<RHIGraphicsPipeline> compile_graphics(const GraphicsPipelineConfig& cfg);

    explicit PipelineCompiler(const SharedRef<RHIDevice>& device);

private:
    SharedRef<RHIDevice> device_ = nullptr;
};

}