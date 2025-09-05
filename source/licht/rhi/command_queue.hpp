#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/command_buffer.hpp"

namespace licht {

enum class RHIQueueType {
    Graphics,
    Compute,
    Transfer,
};

class RHICommandQueue {
public:
    virtual void submit(RHICommandBufferHandle command_buffer) = 0;

    virtual void wait_idle() = 0;

public:
    virtual ~RHICommandQueue() = default;
};

}  //namespace licht