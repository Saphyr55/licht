#pragma once

#include <memory>

#include "licht/core/memory/memory.hpp"

namespace licht {

template <typename DeleterType>
class DeleterHolder {
public:
    template <typename ResourceType>
    void invoke_deleter(ResourceType* resource) noexcept {
        deleter_(resource);
    }

    explicit DeleterHolder(const DeleterType& deleter)
        : deleter_(deleter) {}

private:
    DeleterType deleter_;
};

template <typename ResourceType>
class DefaultDeleter {
public:
    void operator()(ResourceType* resource) noexcept { destroy(resource); }

    void destroy(ResourceType* resource) noexcept {
        if (resource) {
            Memory::delete_resource(resource);
        }
    }

    virtual ~DefaultDeleter() = default;
};

}  // namespace licht