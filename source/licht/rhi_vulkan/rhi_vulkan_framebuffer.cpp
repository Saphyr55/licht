#include "rhi_vulkan_framebuffer.hpp"
#include "licht/rhi/framebuffer.hpp"

namespace licht {

VkFramebuffer& RHIVulkanFramebuffer::get_handle() {
    return handle_;
}

RHIVulkanFramebuffer::RHIVulkanFramebuffer(const RHIFramebufferDescription& p_description)
    : description_(p_description) {
}

}  //namespace licht