#include "rhi_vulkan_module.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/platform/display.hpp"
#include "licht/rhi_vulkan/vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_queue.hpp"
#include "licht/rhi_vulkan/vulkan_swapchain.hpp"
#include "licht/rhi_vulkan/vulkan_graphics_pipeline.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_render_pass.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

void RHIVulkanModule::initialize() {
    LLOG_INFO("[RHIVulkanModule::initialize]", "Initializing Vulkan RHI module...");

    LLOG_FATAL_WHEN(!Display::get_default().is_valid(Display::MAIN_WINDOW_HANDLE), "[RHIVulkanModule::initialize]",
                    "Failed to retrieve a valid window handle. Ensure a window is created before initializing the Vulkan RHI module.");

    void* window_handle = Display::get_default().get_native_window_handle(Display::MAIN_WINDOW_HANDLE);

    context_ = vulkan_context_create(window_handle);
    LLOG_FATAL_WHEN(!context_, "[RHIVulkanModule::initialize]", "Failed to create Vulkan RHI context.");
}

void RHIVulkanModule::tick() {

    VulkanQueue& graphics_queue = context_->device->query_queue(VulkanQueueFamilyType::Graphics);
    VulkanQueue& present_queue = context_->device->query_queue(VulkanQueueFamilyType::Present);

    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    uint32 image_index = 0;
    VkResult acquire_next_image_result = VulkanAPI::lvkAcquireNextImageKHR(context_->device->get_handle(), context_->swapchain, UINT64_MAX, context_->image_available_semaphores[context_->current_frame], VK_NULL_HANDLE, &image_index);
    if (acquire_next_image_result == VK_ERROR_OUT_OF_DATE_KHR || acquire_next_image_result == VK_SUBOPTIMAL_KHR || window_resized_) {
        vulkan_swapchain_recreate(context_);
        window_resized_ = false;
        return;
    } else if (acquire_next_image_result != VK_SUBOPTIMAL_KHR) {
        LICHT_VULKAN_CHECK(acquire_next_image_result)
    }

    LICHT_VULKAN_CHECK(VulkanAPI::lvkResetFences(context_->device->get_handle(), 1, &context_->in_flight_fences[context_->current_frame]));

    VulkanAPI::lvkResetCommandBuffer(context_->command_buffers[context_->current_frame], 0);
    
    vulkan_command_buffer_begin(context_, context_->command_buffers[context_->current_frame]);
    vulkan_render_pass_begin(context_, context_->command_buffers[context_->current_frame], image_index);
    {
        vulkan_pipeline_bind(context_, context_->command_buffers[context_->current_frame]);
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float32>(context_->swapchain_extent.width);
        viewport.height = static_cast<float32>(context_->swapchain_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VulkanAPI::lvkCmdSetViewport(context_->command_buffers[context_->current_frame], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = context_->swapchain_extent;
        VulkanAPI::lvkCmdSetScissor(context_->command_buffers[context_->current_frame], 0, 1, &scissor);

        vulkan_command_buffer_draw(context_, context_->command_buffers[context_->current_frame], 3, 1, 0, 0);
    }
    vulkan_render_pass_end(context_, context_->command_buffers[context_->current_frame]);

    vulkan_command_buffer_end(context_, context_->command_buffers[context_->current_frame]);

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {context_->image_available_semaphores[context_->current_frame]};
    VkPipelineStageFlags pipeline_stages_flags[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = pipeline_stages_flags;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &context_->command_buffers[context_->current_frame];

    VkSemaphore signal_semaphores[] = {context_->render_finished_semaphores[context_->current_frame]};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkQueueSubmit(graphics_queue.get_handle(), 1, &submit_info, context_->in_flight_fences[context_->current_frame]));

    VkSwapchainKHR swapchains[] = {context_->swapchain};

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr; // Optional

    LICHT_VULKAN_CHECK(VulkanAPI::lvkQueuePresentKHR(present_queue.get_handle(), &present_info));
    LICHT_VULKAN_CHECK(VulkanAPI::lvkWaitForFences(context_->device->get_handle(), 1, &context_->in_flight_fences[context_->current_frame], VK_TRUE, UINT64_MAX));

    context_->current_frame = (context_->current_frame  + 1) % VulkanContext::MaxFrame;
}

void RHIVulkanModule::shutdown() {
    LICHT_VULKAN_CHECK(VulkanAPI::lvkDeviceWaitIdle(context_->device->get_handle()));

    vulkan_context_destroy(context_);

    LLOG_INFO("[RHIVulkanModule::shutdown]", "Shutting down Vulkan RHI module...");
}

}  //namespace licht