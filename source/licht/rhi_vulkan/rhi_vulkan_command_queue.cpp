#include "rhi_vulkan_command_queue.hpp"
#include "licht/core/collection/array.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi_vulkan/rhi_sync.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_swapchain.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

namespace licht {

void RHIVulkanCommandQueue::wait_idle() {
    LICHT_VULKAN_CHECK(VulkanAPI::lvkQueueWaitIdle(queue_));
}

void RHIVulkanCommandQueue::present(RHISwapchainHandle swapchain,
                                    const Array<RHISemaphoreHandle>& wait_semaphores,
                                    uint32 frame_index) {
    RHIVulkanSwapchainRef vk_swapchain = static_ref_cast<RHIVulkanSwapchain>(swapchain);

    Array<VkSemaphore> vk_wait_semaphores(wait_semaphores.size());
    for (RHISemaphoreHandle& sem : wait_semaphores) {
        RHIVulkanSemaphore* vk_sem_ref = static_cast<RHIVulkanSemaphore*>(sem.get_resource());
        vk_wait_semaphores.push_back(vk_sem_ref->get_handle());
    }

    VkSwapchainKHR swapchains[] = {vk_swapchain->get_handle()};

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = static_cast<uint32>(wait_semaphores.size());
    present_info.pWaitSemaphores = vk_wait_semaphores.data();
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &frame_index;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkQueuePresentKHR(queue_, &present_info));
}

void RHIVulkanCommandQueue::submit(const Array<RHICommandBufferHandle>& command_buffers,
                                   const Array<RHISemaphoreHandle>& wait_semaphores,
                                   const Array<RHISemaphoreHandle>& signal_semaphores,
                                   RHIFenceHandle fence) {
    Array<VkSemaphore> vk_wait_semaphores(wait_semaphores.size());
    for (RHISemaphoreHandle& sem : wait_semaphores) {
        RHIVulkanSemaphore* vk_sem_ref = static_cast<RHIVulkanSemaphore*>(sem.get_resource());
        vk_wait_semaphores.push_back(vk_sem_ref->get_handle());
    }

    Array<VkSemaphore> vk_signal_semaphores(signal_semaphores.size());
    for (RHISemaphoreHandle& sem : signal_semaphores) {
        RHIVulkanSemaphore* vk_sem_ref = static_cast<RHIVulkanSemaphore*>(sem.get_resource());
        vk_signal_semaphores.push_back(vk_sem_ref->get_handle());
    }

    Array<VkCommandBuffer> vk_command_buffers(command_buffers.size());
    for (RHICommandBufferHandle& cmd : command_buffers) {
        RHIVulkanCommandBuffer* vk_cmd_ref = static_cast<RHIVulkanCommandBuffer*>(cmd.get_resource());
        vk_command_buffers.push_back(vk_cmd_ref->get_handle());
    }

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = static_cast<uint32>(vk_wait_semaphores.size());
    submit_info.pWaitSemaphores = vk_wait_semaphores.data();

    Array<VkPipelineStageFlags> wait_stages(vk_wait_semaphores.size());
    for (size_t i = 0; i < vk_wait_semaphores.size(); ++i) {
        wait_stages[i] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    submit_info.pWaitDstStageMask = wait_stages.data();

    submit_info.commandBufferCount = static_cast<uint32>(vk_command_buffers.size());
    submit_info.pCommandBuffers = vk_command_buffers.data();

    submit_info.signalSemaphoreCount = static_cast<uint32>(vk_signal_semaphores.size());
    submit_info.pSignalSemaphores = vk_signal_semaphores.data();

    VkFence vk_fence = fence ? static_cast<RHIVulkanFence*>(fence.get_resource())->get_handle() : VK_NULL_HANDLE;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkQueueSubmit(queue_, 1, &submit_info, vk_fence));
}

RHIVulkanCommandQueue::RHIVulkanCommandQueue(VulkanContext& context, VkQueue queue, RHIQueueType type)
    : context_(context), queue_(queue), type_(type) {
}

}  //namespace licht