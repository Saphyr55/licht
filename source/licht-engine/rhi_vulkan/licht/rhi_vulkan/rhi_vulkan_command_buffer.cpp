#include "rhi_vulkan_command_buffer.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_framebuffer.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_pipeline.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

void RHIVulkanCommandBuffer::begin() {
    VkCommandBufferBeginInfo command_buffer_begin_info = {};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = 0;
    command_buffer_begin_info.pInheritanceInfo = nullptr;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkBeginCommandBuffer(command_buffer_, &command_buffer_begin_info));
}

void RHIVulkanCommandBuffer::end() {
    LICHT_VULKAN_CHECK(VulkanAPI::lvkEndCommandBuffer(command_buffer_));
}

void RHIVulkanCommandBuffer::bind_pipeline(RHIPipelineHandle pipeline) {
    RHIVulkanPipelineRef vulkan_graphics_pipeline = static_ref_cast<RHIVulkanPipeline>(pipeline);
    VulkanAPI::lvkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_graphics_pipeline->pipeline());
}

void RHIVulkanCommandBuffer::set_scissors(const Rect2D* scissors, uint32 count) {
    Array<VkRect2D> vk_scissors;
    vk_scissors.resize(count);
    for (usize i = 0; i < count; i++) {
        vk_scissors[i].offset.x = static_cast<int32>(scissors[i].x);
        vk_scissors[i].offset.y = static_cast<int32>(scissors[i].y);
        vk_scissors[i].extent.width = static_cast<uint32>(scissors[i].width);
        vk_scissors[i].extent.height = static_cast<uint32>(scissors[i].height);
    }
    VulkanAPI::lvkCmdSetScissor(command_buffer_, 0, count, vk_scissors.data());
}

void RHIVulkanCommandBuffer::set_viewports(const Viewport* viewports, uint32 count) {
    Array<VkViewport> vk_viewports;
    vk_viewports.resize(count);
    for (uint32 i = 0; i < count; i++) {
        vk_viewports[i].x = viewports[i].x;
        vk_viewports[i].y = viewports[i].y;
        vk_viewports[i].width = viewports[i].width;
        vk_viewports[i].height = viewports[i].height;
        vk_viewports[i].minDepth = viewports[i].min_depth;
        vk_viewports[i].maxDepth = viewports[i].max_depth;
    }
    VulkanAPI::lvkCmdSetViewport(command_buffer_, 0, count, vk_viewports.data());
}

void RHIVulkanCommandBuffer::begin_render_pass(const RHIRenderPassBeginInfo& begin_info) {

    RHIVulkanRenderPassRef vk_render_pass = static_ref_cast<RHIVulkanRenderPass>(begin_info.render_pass);
    RHIVulkanFramebufferRef vk_framebuffer = static_ref_cast<RHIVulkanFramebuffer>(begin_info.framebuffer);

    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = vk_render_pass->get_handle();
    render_pass_begin_info.framebuffer = vk_framebuffer->get_handle();
    render_pass_begin_info.renderArea.offset = {static_cast<int32>(begin_info.area.x), static_cast<int32>(begin_info.area.y)};
    render_pass_begin_info.renderArea.extent = { static_cast<uint32>(begin_info.area.width), static_cast<uint32>(begin_info.area.height) };

    VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clear_color;

    VulkanAPI::lvkCmdBeginRenderPass(command_buffer_, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void RHIVulkanCommandBuffer::end_render_pass() {
    VulkanAPI::lvkCmdEndRenderPass(command_buffer_);
}

void RHIVulkanCommandBuffer::draw(const RHIDrawCommand& command) {
    VulkanAPI::lvkCmdDraw(command_buffer_, command.vertex_count, command.instance_count, command.first_vertex, command.first_instance);
}

RHICommandBufferHandle RHIVulkanCommandAllocator::open(uint32 index) {
    VkCommandBuffer command_buffer = command_buffers_[index];
    return new_ref<RHIVulkanCommandBuffer>(command_buffer);
}

void RHIVulkanCommandAllocator::reset_command_buffer(RHICommandBufferHandle command_buffer) {
    RHIVulkanCommandBufferRef vk_command_buffer = static_ref_cast<RHIVulkanCommandBuffer>(command_buffer);
    VulkanAPI::lvkResetCommandBuffer(vk_command_buffer->get_handle(), 0);
}

VkCommandPool& RHIVulkanCommandAllocator::get_command_pool() {
    return command_pool_;
}

const Array<VkCommandBuffer>& RHIVulkanCommandAllocator::get_command_buffers() const {
    return command_buffers_;
}

void RHIVulkanCommandAllocator::initialize_command_pool() {
    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_create_info.queueFamilyIndex = context_.physical_device_info.graphics_queue_index;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateCommandPool(context_.device, &command_pool_create_info, context_.allocator, &command_pool_));
}

void RHIVulkanCommandAllocator::allocate_command_buffers() {
    command_buffers_.resize(count_);

    VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.commandPool = command_pool_;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = count_;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkAllocateCommandBuffers(context_.device, &command_buffer_allocate_info, command_buffers_.data()));
}

void RHIVulkanCommandAllocator::destroy() {
    VulkanAPI::lvkDestroyCommandPool(context_.device, command_pool_, context_.allocator);
}

RHIVulkanCommandAllocator::RHIVulkanCommandAllocator(VulkanContext& context, uint32 count)
    : context_(context)
    , count_(count) {
}

}  //namespace licht