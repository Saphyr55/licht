#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/rhi.hpp"

namespace licht {

struct RHICommand {};

struct RHIDrawCommand : RHICommand {
    uint32 vertex_count;
    uint32 instance_count; 
    uint32 first_vertex; 
    uint32 first_instance;
};

class RHICommandBuffer {
public:
    virtual void begin() = 0;

    virtual void end() = 0;

    virtual void set_scissors(const Rect2D* scissors, uint32 count) = 0;

    virtual void set_viewports(const Viewport* viewports, uint32 count) = 0;

    virtual void draw(const RHIDrawCommand& command) = 0;
};

using RHICommandBufferHandle = SharedRef<RHICommandBuffer>;

}  //namespace licht