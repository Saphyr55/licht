#pragma once

namespace licht {

template <typename T>
class VulkanResource {
public:
    virtual T create() = 0;

    virtual void destroy() = 0;

private:
};

}  //namespace licht