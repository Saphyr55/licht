#pragma once

#include <memory>

#include "licht/core/memory/memory.hpp"

namespace licht {

template <typename DeleterType>
class DeleterHolder {
public:
    template <typename ResourceType>
    void invoke_deleter(ResourceType* resource) {
        deleter_.destroy(resource);
    }

    explicit DeleterHolder(DeleterType&& deleter)
        : deleter_(std::move(deleter)) {}

private:
    DeleterType deleter_;
};

template <typename ResourceType>
class DefaultDeleter {
public:
    void operator()(ResourceType* ptr) const { destroy(ptr); }

    void destroy(ResourceType* ptr) const {
        if (ptr) {
            ::delete ptr;
        }
    }

    virtual ~DefaultDeleter() = default;
};

}  // namespace licht