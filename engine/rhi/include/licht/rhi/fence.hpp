#pragma once

namespace licht {

class RHIFence {
public:
    virtual bool is_signaled() = 0;

    virtual ~RHIFence() = default;
};

}  //namespace licht
