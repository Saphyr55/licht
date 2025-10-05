#pragma once

#include <concepts>

namespace licht {

template<typename ResourceType>
concept CRHIVulkanResource = requires (ResourceType* resource) {
    { resource->initialize() } -> std::same_as<void>;
    { resource->destroy() } -> std::same_as<void>;
};

}

