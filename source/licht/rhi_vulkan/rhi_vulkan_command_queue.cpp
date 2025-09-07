#include "rhi_vulkan_command_queue.hpp"
#include "licht/core/collection/array.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi_vulkan/rhi_sync.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_swapchain.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

void RHIVulkanCommandQueue::wait_idle() {
    LICHT_VULKAN_CHECK(VulkanAPI::lvkQueueWaitIdle(queue_));
}

void RHIVulkanCommandQueue::submit(const Array<RHICommandBufferHandle>& command_buffers, RHIFrameContext& frame_context) {

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    // Command buffers
    Array<VkCommandBuffer> vk_command_buffers(command_buffers.size());
    for (RHICommandBufferHandle& cmd : command_buffers) {
        RHIVulkanCommandBuffer* vk_cmd_ref = static_cast<RHIVulkanCommandBuffer*>(cmd.get_resource());
        vk_command_buffers.append(vk_cmd_ref->get_handle());
    }

    submit_info.commandBufferCount = static_cast<uint32>(vk_command_buffers.size());
    submit_info.pCommandBuffers = vk_command_buffers.data();

    // Wait semaphores
    SharedRef<RHIVulkanSemaphore> rhi_vk_semaphore_frame_available = static_ref_cast<RHIVulkanSemaphore>(frame_context.current_frame_available_semaphore());
    VkSemaphore& vk_semaphore_frame_available = rhi_vk_semaphore_frame_available->get_handle();

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &vk_semaphore_frame_available;

    SharedRef<RHIVulkanSemaphore> rhi_vk_semaphore_render_finished = static_ref_cast<RHIVulkanSemaphore>(frame_context.current_render_finished_semaphore());
    VkSemaphore& vk_semaphore_render_finished = rhi_vk_semaphore_render_finished->get_handle();
  
    // Signal semaphores
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &vk_semaphore_render_finished;

    // Waiting pipeline stage policy
    VkPipelineStageFlags wait_stages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.pWaitDstStageMask = wait_stages;

    SharedRef<RHIVulkanFence> fence = static_ref_cast<RHIVulkanFence>(frame_context.current_in_flight_fence());

    LICHT_VULKAN_CHECK(VulkanAPI::lvkQueueSubmit(queue_, 1, &submit_info, fence->get_handle()));
}

void RHIVulkanCommandQueue::present(RHISwapchainHandle swapchain, RHIFrameContext& frame_context) {
    
    RHIVulkanSwapchainRef vk_swapchain = static_ref_cast<RHIVulkanSwapchain>(swapchain);
    SharedRef<RHIVulkanSemaphore> vk_sem_ref = static_ref_cast<RHIVulkanSemaphore>(frame_context.current_render_finished_semaphore());
    VkSemaphore vksemaphore = vk_sem_ref->get_handle();

    VkSwapchainKHR swapchains[] = {vk_swapchain->get_handle()};

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &vksemaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &frame_context.frame_index;

    VkResult result = VulkanAPI::lvkQueuePresentKHR(queue_, &present_info);
    switch (result) {
        case VK_SUCCESS:
            frame_context.success = true;
            break;
        case VK_SUBOPTIMAL_KHR:
            frame_context.success = true;
            frame_context.suboptimal = true;
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            frame_context.out_of_date = true;
            break;
        default:
            LICHT_VULKAN_CHECK(result);
    }
}

RHIVulkanCommandQueue::RHIVulkanCommandQueue(VulkanContext& context, VkQueue queue, RHIQueueType type)
    : context_(context), queue_(queue), type_(type) {
}

}  //namespace licht