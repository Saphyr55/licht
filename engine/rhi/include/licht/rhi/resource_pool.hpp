#pragma once

#include "licht/rhi/resource.hpp"
namespace licht {

template <typename T>
class ResourcePool {
public:
    ResourcePool(Allocator& allocator)
        : allocator_(allocator) {}

public:
    RHIResourceHandle<T> new_resource(T&& resource);

    T* get_resource(RHIResourceHandle<T> handle);

    void release_resource(RHIResourceHandle<T> handle);

private:
    Allocator& allocator_;
};

}  //namespace licht