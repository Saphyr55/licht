#include "vulkan_queue.hpp"

#include "licht/rhi_vulkan/vulkan_loader.hpp"

namespace licht {
    
VulkanQueue::VulkanQueue(VulkanQueueFamilyType p_type, uint32 p_queue_family_index)
    : type_(p_type)
    , queue_family_index_(p_queue_family_index) {
}

}