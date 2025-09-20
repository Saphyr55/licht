#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/string/string.hpp"

namespace licht {

class RHIResource {
public:
    virtual ~RHIResource() = default;
};

template<typename Resource>
class Handle {
private:
    virtual Resource* get_resource() = 0;
};

using RHIResourceHandle = SharedRef<RHIResource>;

}  //namespace licht