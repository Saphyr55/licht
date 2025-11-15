#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/rhi/rhi_forwards.hpp"

namespace licht {

struct RenderGraphResource;
struct RenderGraphPass;

struct RenderGraphNode {
    RHIRenderPass* render_pass;
    RHIFramebuffer* framebuffer;

    RenderGraphPass* pass;

    Array<RenderGraphResource*> inputs;
    Array<RenderGraphResource*> outputs;

    Array<RenderGraphNode*> edges;

    StringRef name;
};

}