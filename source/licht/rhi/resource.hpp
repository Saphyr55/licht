#pragma once

#include "licht/core/memory/shared_ref.hpp"

namespace licht {

class RHIResource {
public:

public:
    RHIResource(const RHIResource&) = delete;
    RHIResource(RHIResource&&) = delete;
    RHIResource& operator=(const RHIResource&) = delete;
    RHIResource& operator=(RHIResource&&) = delete;
};

using RHIResourceHandle = SharedRef<RHIResource>;

}  //namespace licht