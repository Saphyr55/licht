#include "licht/rhi_vulkan/vulkan_command_queue.hpp"
#include "licht/core/containers/array.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi_vulkan/vulkan_sync.hpp"
#include "licht/rhi_vulkan/vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_swapchain.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

RHIVulkanCommandQueue::RHIVulkanCommandQueue(VulkanContext& context, VkQueue queue, RHIQueueType type, uint32 queue_family_index, bool is_present_mode_supported)
    : context_(context), queue_(queue), type_(type), queue_family_index_(queue_family_index), is_present_mode_supported_(is_present_mode_supported) {
}

bool RHIVulkanCommandQueue::is_present_mode() {
    return is_present_mode_supported_;
}

void RHIVulkanCommandQueue::wait_idle() {
    LICHT_VULKAN_CHECK(VulkanAPI::lvkQueueWaitIdle(queue_));
}

void RHIVulkanCommandQueue::submit(const Array<RHICommandBufferRef>& command_buffers, 
                                   const Array<RHISemaphoreRef>& wait_semaphores,
                                   const Array<RHISemaphoreRef>& signal_semaphores,
                                   const RHIFenceRef fence) {
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Command buffers
    Array<VkCommandBuffer> vk_command_buffers(command_buffers.size());
    for (RHICommandBufferRef& cmd : command_buffers) {
        RHIVulkanCommandBufferRef vk_cmd_ref = static_ref_cast<RHIVulkanCommandBuffer>(cmd);
        vk_command_buffers.append(vk_cmd_ref->get_handle());
    }

    submit_info.commandBufferCount = static_cast<uint32>(vk_command_buffers.size());
    submit_info.pCommandBuffers = vk_command_buffers.data();

    // Wait semaphores
    Array<VkSemaphore> wait_vksemaphores(wait_semaphores.size());
    for (RHISemaphoreRef sem : wait_semaphores) {
        wait_vksemaphores.append(static_ref_cast<RHIVulkanSemaphore>(sem)->get_handle());
    }

    submit_info.waitSemaphoreCount = wait_vksemaphores.size();
    submit_info.pWaitSemaphores = wait_vksemaphores.data();

    // Signal semaphores

    Array<VkSemaphore> signal_vksemaphores(signal_semaphores.size());
    for (RHISemaphoreRef sem : signal_semaphores) {
        signal_vksemaphores.append(static_ref_cast<RHIVulkanSemaphore>(sem)->get_handle());
    }

    submit_info.signalSemaphoreCount = signal_vksemaphores.size();
    submit_info.pSignalSemaphores = signal_vksemaphores.data();

    // Waiting pipeline stage policy
    Array<VkPipelineStageFlags> wait_stages(wait_semaphores.size());
    for (size_t i = 0; i < wait_semaphores.size(); i++) {
        wait_stages.append(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }

    submit_info.pWaitDstStageMask = wait_stages.data();
    // Fence
    SharedRef<RHIVulkanFence> rhi_vkfence = static_ref_cast<RHIVulkanFence>(fence);
    VkFence vkfence = VK_NULL_HANDLE;
    if (rhi_vkfence) {
        vkfence = rhi_vkfence->get_handle();
    }

    // Submit
    LICHT_VULKAN_CHECK(VulkanAPI::lvkQueueSubmit(queue_, 1, &submit_info, vkfence));
}

void RHIVulkanCommandQueue::present(RHISwapchainRef swapchain, RHIFrameContext& frame_context) {
    RHIVulkanSwapchainRef vk_swapchain = static_ref_cast<RHIVulkanSwapchain>(swapchain);
    SharedRef<RHIVulkanSemaphore> vk_sem_ref = static_ref_cast<RHIVulkanSemaphore>(frame_context.current_render_finished_semaphore());
    VkSemaphore vksemaphore = vk_sem_ref->get_handle();

    VkSwapchainKHR swapchains[] = {vk_swapchain->get_handle()};

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &vksemaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &frame_context.frame_index;

    VkResult qresult = VulkanAPI::lvkQueuePresentKHR(queue_, &present_info);
    switch (qresult) {
        case VK_SUCCESS: {
            frame_context.success = true;
            break;
        }
        case VK_SUBOPTIMAL_KHR: {
            frame_context.success = true;
            frame_context.suboptimal = true;
            break;
        }
        case VK_ERROR_OUT_OF_DATE_KHR: {
            frame_context.out_of_date = true;
            break;
        }
        default: {
            LICHT_VULKAN_CHECK(qresult);
            break;
        }
    }
}



}  //namespace licht