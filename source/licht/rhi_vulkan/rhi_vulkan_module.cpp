#include "rhi_vulkan_module.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/platform/display.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_context.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_graphics_pipeline.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_loader.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_render_pass.hpp"
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
    LICHT_VULKAN_CHECK(context_->rhi.licht_vkResetFences(context_->device, 1, &context_->in_flight_fence));

    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    uint32 image_index = 0;
    LICHT_VULKAN_CHECK(context_->rhi.licht_vkAcquireNextImageKHR(context_->device, context_->swapchain, UINT64_MAX, context_->image_available_semaphore, VK_NULL_HANDLE, &image_index));

    context_->rhi.licht_vkResetCommandBuffer(context_->command_buffer, 0);
    
    vulkan_command_buffer_begin(context_, context_->command_buffer);
    vulkan_render_pass_begin(context_, context_->command_buffer, image_index);
    {
        vulkan_pipeline_bind(context_, context_->command_buffer);
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float32>(context_->swapchain_extent.width);
        viewport.height = static_cast<float32>(context_->swapchain_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        context_->rhi.licht_vkCmdSetViewport(context_->command_buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = context_->swapchain_extent;
        context_->rhi.licht_vkCmdSetScissor(context_->command_buffer, 0, 1, &scissor);

        vulkan_command_buffer_draw(context_, context_->command_buffer, 3, 1, 0, 0);
    }
    vulkan_render_pass_end(context_, context_->command_buffer);

    vulkan_command_buffer_end(context_, context_->command_buffer);

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {context_->image_available_semaphore};
    VkPipelineStageFlags pipeline_stages_flags[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = pipeline_stages_flags;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &context_->command_buffer;

    VkSemaphore signal_semaphores[] = {context_->render_finished_semaphore};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    LICHT_VULKAN_CHECK(context_->rhi.licht_vkQueueSubmit(context_->graphics_queue, 1, &submit_info, context_->in_flight_fence));

    VkSwapchainKHR swapchains[] = {context_->swapchain};

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr; // Optional

    LICHT_VULKAN_CHECK(context_->rhi.licht_vkQueuePresentKHR(context_->present_queue, &present_info));
    LICHT_VULKAN_CHECK(context_->rhi.licht_vkWaitForFences(context_->device, 1, &context_->in_flight_fence, VK_TRUE, UINT64_MAX));
}

void RHIVulkanModule::shutdown() {
    LICHT_VULKAN_CHECK(context_->rhi.licht_vkDeviceWaitIdle(context_->device));

    vulkan_context_destroy(context_);

    LLOG_INFO("[RHIVulkanModule::shutdown]", "Shutting down Vulkan RHI module...");
}

}  //namespace licht