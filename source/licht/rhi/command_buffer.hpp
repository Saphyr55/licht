#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/pipeline/pipeline.hpp"
#include "licht/rhi/rhi.hpp"

namespace licht {

struct RHICommand {};

struct RHIDrawCommand : RHICommand {
    uint32 vertex_count;
    uint32 instance_count;
    uint32 first_vertex;
    uint32 first_instance;
};

struct RHIRenderPassBeginInfo {
    RHIRenderPassHandle render_pass;
    RHIFramebufferHandle framebuffer;
    Rect2D area;
};

class RHICommandBuffer {
public:
    virtual void begin() = 0;

    virtual void end() = 0;

    virtual void begin_render_pass(const RHIRenderPassBeginInfo& render_pass_begin_info) = 0;

    virtual void end_render_pass() = 0;

    virtual void bind_pipeline(RHIPipelineHandle pipeline) = 0;

    virtual void set_scissors(const Rect2D* scissors, uint32 count) = 0;

    virtual void set_viewports(const Viewport* viewports, uint32 count) = 0;

    virtual void draw(const RHIDrawCommand& command) = 0;
};

using RHICommandBufferHandle = SharedRef<RHICommandBuffer>;

class RHICommandAllocator {
public:
    virtual RHICommandBufferHandle open() = 0;

    virtual void reset_command_buffer(RHICommandBufferHandle command_buffer) = 0;
};

using RHICommandAllocatorRef = SharedRef<RHICommandAllocator>;

}  //namespace licht