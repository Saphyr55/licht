#pragma once

#include <vulkan/vulkan_core.h>

namespace licht {

struct VulkanContext;

class VulkanInstance {
public:

    void initialize(const VkApplicationInfo& p_info, const VkInstanceCreateInfo& p_create_info);

    void destroy();

public:
    inline VkInstance get_handle() {
        return handle_;
    }

    inline operator VkInstance() {
        return get_handle();
    }

    inline VkAllocationCallbacks* get_allocator() {
        return allocator_;
    }

    inline void set_allocator(VkAllocationCallbacks* p_allocator) {
        allocator_ = p_allocator;
    }

public:
    VulkanInstance() = default;

private:
    VkAllocationCallbacks* allocator_ = nullptr;
    VkInstance handle_ = VK_NULL_HANDLE;
};

}