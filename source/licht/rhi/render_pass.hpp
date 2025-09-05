#pragma once

#include "licht/core/memory/shared_ref.hpp"
namespace licht {

class RHIRenderPass {
public:
    virtual void begin() = 0;

    virtual void end() = 0;
};

using RHIRenderPassHandle = SharedRef<RHIRenderPass>;

}