#pragma once

#include <vulkan/vulkan_core.h>

#include "licht/core/defines.hpp"

namespace licht {

enum class VulkanQueueFamilyType {
    Present,
    Graphics,
    Unknown,
};

class VulkanQueue {
public:
    void initialize(VkDevice p_device);

public:
    inline VulkanQueueFamilyType get_type() {
        return type_;
    }

    inline uint32 get_queue_family_index_() {
        return queue_family_index_;
    }

    inline VkQueue get_handle() {
        return handle_;
    }

    inline bool is_valid() {
        return handle_ != VK_NULL_HANDLE;
    }

public:
    VulkanQueue(VulkanQueueFamilyType p_type, uint32 p_queue_family_index);

private:
    VulkanQueueFamilyType type_ = VulkanQueueFamilyType::Unknown;
    VkQueue handle_ = VK_NULL_HANDLE;
    uint32 queue_family_index_ = 0;
};

}  //namespace licht
