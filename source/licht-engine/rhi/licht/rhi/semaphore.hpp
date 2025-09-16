#pragma once

#include "licht/core/memory/shared_ref.hpp"

namespace licht {

class RHISemaphore {
public:
    virtual ~RHISemaphore() = default;
};

using RHISemaphoreHandle = SharedRef<RHISemaphore>;

}
