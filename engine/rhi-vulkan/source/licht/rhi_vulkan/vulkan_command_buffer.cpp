#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/linear_allocator.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/vulkan_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_command_queue.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_description_set.hpp"
#include "licht/rhi_vulkan/vulkan_framebuffer.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_graphics_pipeline.hpp"
#include "licht/rhi_vulkan/vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_texture.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

static void pick_pipeline_stages_and_access(VkQueueFlags queueFlags,
                                            VkImageLayout oldLayout,
                                            VkImageLayout newLayout,
                                            VkPipelineStageFlags& outSrcStage,
                                            VkPipelineStageFlags& outDstStage,
                                            VkAccessFlags& outSrcAccess,
                                            VkAccessFlags& outDstAccess) {
    // defaults
    outSrcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    outDstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    outSrcAccess = 0;
    outDstAccess = 0;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        outSrcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        outDstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        outSrcAccess = 0;
        outDstAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
        return;
    }

    if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        outSrcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        outSrcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;

        // prefer shader stages if the queue supports them
        if (queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            outDstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            outDstAccess = VK_ACCESS_SHADER_READ_BIT;
        } else if (queueFlags & VK_QUEUE_COMPUTE_BIT) {
            outDstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            outDstAccess = VK_ACCESS_SHADER_READ_BIT;
        } else {
            // fallback for transfer-only queue: use transfer stage.
            // NOTE: a later barrier on the graphics queue will be required
            outDstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            outDstAccess = VK_ACCESS_TRANSFER_READ_BIT;
            // You should log a warning here: the image is transitioned to shader layout
            // on a transfer queue but shader read visibility requires extra sync on graphics queue.
        }
        return;
    }

    // use transfer stages if unsure
    outSrcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    outDstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    outSrcAccess = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    outDstAccess = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
}

void VulkanCommandBuffer::begin(RHICommandBufferUsageFlags usage) {
    VkCommandBufferUsageFlags flags = 0;

    if ((usage & RHICommandBufferUsageFlags::OneTimeSubmit) == RHICommandBufferUsageFlags::OneTimeSubmit) {
        flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }

    if ((usage & RHICommandBufferUsageFlags::RenderPassContinue) == RHICommandBufferUsageFlags::RenderPassContinue) {
        flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    }

    if ((usage & RHICommandBufferUsageFlags::SimultaneousUse) == RHICommandBufferUsageFlags::SimultaneousUse) {
        flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    }

    VkCommandBufferBeginInfo command_buffer_begin_info = {};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = flags;
    command_buffer_begin_info.pInheritanceInfo = nullptr;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkBeginCommandBuffer(command_buffer_, &command_buffer_begin_info));
}

void VulkanCommandBuffer::end() {
    LICHT_VULKAN_CHECK(VulkanAPI::lvkEndCommandBuffer(command_buffer_));
}

void VulkanCommandBuffer::bind_pipeline(RHIGraphicsPipeline* pipeline) {
    VulkanGraphicsPipeline* vulkan_graphics_pipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
    VulkanAPI::lvkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_graphics_pipeline->get_handle());
}

void VulkanCommandBuffer::bind_shader_resource_group(RHIGraphicsPipeline* pipeline, const Array<RHIShaderResourceGroup*>& groups) {
    static constexpr auto mapping_group = [](RHIShaderResourceGroup*& descriptor_set) -> VkDescriptorSet {
        VulkanShaderResourceGroup* vk_descriptor_set = static_cast<VulkanShaderResourceGroup*>(descriptor_set);
        return vk_descriptor_set->get_handle();
    };

    Array<VkDescriptorSet> vk_descriptor_sets = groups.map<VkDescriptorSet>(mapping_group);
    VulkanGraphicsPipeline* vk_pipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);

    VulkanAPI::lvkCmdBindDescriptorSets(
        command_buffer_,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        vk_pipeline->get_layout(),
        0,
        vk_descriptor_sets.size(),
        vk_descriptor_sets.data(),
        0,
        nullptr);
}

void VulkanCommandBuffer::bind_vertex_buffers(const Array<RHIBuffer*>& buffers) {
    Array<VkDeviceSize> offsets;
    offsets.resize(buffers.size(), 0);

    Array<VkBuffer> vk_buffers = buffers.map<VkBuffer>([](RHIBuffer* buffer_handle) -> VkBuffer {
        return static_cast<VulkanBuffer*>(buffer_handle)->get_handle();
    });

    VulkanAPI::lvkCmdBindVertexBuffers(
        command_buffer_, 
        0,
        buffers.size(), 
        vk_buffers.data(), 
        offsets.data());
}

void VulkanCommandBuffer::bind_index_buffer(RHIBuffer* buffer) {
    VkBuffer vkbuffer = static_cast<VulkanBuffer*>(buffer)->get_handle();
    VulkanAPI::lvkCmdBindIndexBuffer(
        command_buffer_, 
        vkbuffer, 
        0, 
        // TODO: Make the type configurable
        VK_INDEX_TYPE_UINT32);
}

void VulkanCommandBuffer::set_scissors(const Rect2D* scissors, uint32 count) {
    Array<VkRect2D> vk_scissors;
    vk_scissors.resize(count);
    for (size_t i = 0; i < count; i++) {
        vk_scissors[i].offset.x = static_cast<int32>(scissors[i].x);
        vk_scissors[i].offset.y = static_cast<int32>(scissors[i].y);
        vk_scissors[i].extent.width = static_cast<uint32>(scissors[i].width);
        vk_scissors[i].extent.height = static_cast<uint32>(scissors[i].height);
    }
    VulkanAPI::lvkCmdSetScissor(
        command_buffer_, 
        0, 
        count, 
        vk_scissors.data());
}

void VulkanCommandBuffer::set_viewports(const Viewport* viewports, uint32 count) {
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
    VulkanAPI::lvkCmdSetViewport(
        command_buffer_, 
        0, 
        count, 
        vk_viewports.data());
}

void VulkanCommandBuffer::transition_texture(const RHITextureBarrier& texture_barrier) {
    VulkanTexture* texture = static_cast<VulkanTexture*>(texture_barrier.texture);

    VkImageMemoryBarrier image_barrier = {};
    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_barrier.oldLayout = vulkan_texture_layout_get(texture_barrier.old_layout);
    image_barrier.newLayout = vulkan_texture_layout_get(texture_barrier.new_layout);
    image_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.image = texture->get_handle();
    // TODO: set correct aspect from texture format
    image_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_barrier.subresourceRange.baseMipLevel = 0;
    image_barrier.subresourceRange.levelCount = 1;
    image_barrier.subresourceRange.baseArrayLayer = 0;
    image_barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage = 0;
    VkPipelineStageFlags destination_stage = 0;
    VkAccessFlags srcAccess = 0;
    VkAccessFlags dstAccess = 0;

    pick_pipeline_stages_and_access(
        vulkan_queue_type_get(queue_type_),
        image_barrier.oldLayout,
        image_barrier.newLayout,
        source_stage,
        destination_stage,
        srcAccess,
        dstAccess);

    image_barrier.srcAccessMask = srcAccess;
    image_barrier.dstAccessMask = dstAccess;

    VulkanAPI::lvkCmdPipelineBarrier(
        command_buffer_,
        source_stage,
        destination_stage,
        0,
        0, nullptr,
        0, nullptr,
        1, &image_barrier);
}

void VulkanCommandBuffer::copy_buffer_to_texture(const RHICopyBufferToTextureCommand& command) {
    VkBuffer vk_source_buffer = static_cast<VulkanBuffer*>(command.source)->get_handle();
    VulkanTexture* vulkan_texture = static_cast<VulkanTexture*>(command.destination);

    VkImage vk_destination_image = vulkan_texture->get_handle();

    uint32_t mip_level_factor = 1u << command.mip_level;

    uint32_t final_width = (command.copy_width > 0) 
        ? command.copy_width : vulkan_texture->get_description().width / mip_level_factor;

    uint32_t final_height = (command.copy_height > 0)
        ? command.copy_height : vulkan_texture->get_description().height / mip_level_factor;

    uint32_t final_depth = command.copy_depth;

    VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT; // TODO: 

    VkBufferImageCopy region = {};
    region.bufferOffset = command.buffer_offset;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    // Image Subresource
    region.imageSubresource.aspectMask = aspect;
    region.imageSubresource.mipLevel = command.mip_level;
    region.imageSubresource.baseArrayLayer = command.base_array_layer;
    region.imageSubresource.layerCount = command.layer_count;
    // Image Region
    region.imageOffset = {command.dest_x_offset, command.dest_y_offset, command.dest_z_offset};
    region.imageExtent = {final_width, final_height, final_depth};

    VulkanAPI::lvkCmdCopyBufferToImage(
        command_buffer_,
        vk_source_buffer,
        vk_destination_image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
}

void VulkanCommandBuffer::copy_buffer(const RHICopyBufferCommand& command) {
    VulkanBuffer* vksource = static_cast<VulkanBuffer*>(command.source);
    VulkanBuffer* vkdestination = static_cast<VulkanBuffer*>(command.destination);

    VkBufferCopy buffer_copy = {};
    buffer_copy.srcOffset = command.source_offset;
    buffer_copy.dstOffset = command.destination_offset;
    buffer_copy.size = command.size;

    VulkanAPI::lvkCmdCopyBuffer(
        command_buffer_, 
        vksource->get_handle(), 
        vkdestination->get_handle(), 
        1, 
        &buffer_copy);
}

void VulkanCommandBuffer::begin_render_pass(const RHIRenderPassBeginInfo& begin_info) {
    VulkanRenderPass* vk_render_pass = static_cast<VulkanRenderPass*>(begin_info.render_pass);
    VulkanFramebuffer* vk_framebuffer = static_cast<VulkanFramebuffer*>(begin_info.framebuffer);

    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = vk_render_pass->get_handle();
    render_pass_begin_info.framebuffer = vk_framebuffer->get_handle();
    render_pass_begin_info.renderArea.offset = {
        static_cast<int32>(begin_info.area.x), 
        static_cast<int32>(begin_info.area.y)
    };
    render_pass_begin_info.renderArea.extent = {
        static_cast<uint32>(begin_info.area.width), 
        static_cast<uint32>(begin_info.area.height)
    };

    VkClearValue clear_color = {{{0.01f, 0.02f, 0.1f, 1.0f}}};
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clear_color;

    VulkanAPI::lvkCmdBeginRenderPass(
        command_buffer_, 
        &render_pass_begin_info, 
        VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::end_render_pass() {
    VulkanAPI::lvkCmdEndRenderPass(command_buffer_);
}

void VulkanCommandBuffer::draw(const RHIDrawCommand& command) {
    VulkanAPI::lvkCmdDraw(
        command_buffer_, 
        command.vertex_count, 
        command.instance_count, 
        command.first_vertex,
        command.first_instance);
}

void VulkanCommandBuffer::draw(const RHIDrawIndexedCommand& command) {
    VulkanAPI::lvkCmdDrawIndexed(
        command_buffer_, 
        command.index_count, 
        command.instance_count, 
        command.first_index, 
        command.vertex_offset, 
        command.first_instance);
}

RHICommandBuffer* RHIVulkanCommandAllocator::open(uint32 index) {
    return upper_command_buffers_[index];
}

void RHIVulkanCommandAllocator::reset_command_buffer(RHICommandBuffer* command_buffer) {
    VulkanCommandBuffer* vk_command_buffer = static_cast<VulkanCommandBuffer*>(command_buffer);
    VulkanAPI::lvkResetCommandBuffer(vk_command_buffer->get_handle(), 0);
}

VkCommandPool& RHIVulkanCommandAllocator::get_command_pool() {
    return command_pool_;
}

void RHIVulkanCommandAllocator::initialize_command_pool() {
    VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (queue_type_ == RHIQueueType::Transfer) {
        flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    }
    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.flags = flags;
    command_pool_create_info.queueFamilyIndex = queue_family_index_;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateCommandPool(
        context_.device, 
        &command_pool_create_info, 
        context_.allocator, 
        &command_pool_));
}

void RHIVulkanCommandAllocator::allocate_command_buffers() {
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.commandPool = command_pool_;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = count_;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkAllocateCommandBuffers(
        context_.device, 
        &command_buffer_allocate_info, 
        command_buffers_.data()));

    for (size_t i = 0; i < count_; i++) {
        upper_command_buffers_[i] = lnew(
            upper_command_buffer_allocator_, 
            VulkanCommandBuffer(command_buffers_[i], queue_type_));
    }
}

void RHIVulkanCommandAllocator::destroy() {
    upper_command_buffer_allocator_.destroy();
    VulkanAPI::lvkDestroyCommandPool(
        context_.device, 
        command_pool_, 
        context_.allocator);
}

RHIVulkanCommandAllocator::RHIVulkanCommandAllocator(VulkanContext& context, const RHICommandAllocatorDescription& description)
    : context_(context)
    , count_(description.count)
    , queue_type_(description.command_queue->get_type())
    , queue_family_index_(1)
    , command_pool_(VK_NULL_HANDLE)
    , command_buffers_(description.count) {
    
    command_buffers_.resize(count_);
    upper_command_buffers_.resize(count_);

    upper_command_buffer_allocator_.initialize(count_ * sizeof(VulkanCommandBuffer));
    queue_family_index_ = static_ref_cast<VulkanCommandQueue>(description.command_queue)
        ->get_queue_family_index();
}

}  //namespace licht