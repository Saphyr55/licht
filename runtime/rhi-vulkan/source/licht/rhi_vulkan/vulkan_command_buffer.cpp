#include "licht/rhi_vulkan/vulkan_command_buffer.hpp"
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
#include "licht/rhi_vulkan/vulkan_command_queue.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_description_set.hpp"
#include "licht/rhi_vulkan/vulkan_framebuffer.hpp"
#include "licht/rhi_vulkan/vulkan_graphics_pipeline.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_texture.hpp"

namespace licht {

static void pick_pipeline_stages_and_access(VkQueueFlags queue_flags,
                                            VkImageLayout old_layout,
                                            VkImageLayout new_layout,
                                            VkPipelineStageFlags& out_src_stage,
                                            VkPipelineStageFlags& out_dst_stage,
                                            VkAccessFlags& out_src_access,
                                            VkAccessFlags& out_dst_access) {
    out_src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    out_dst_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    out_src_access = 0;
    out_dst_access = 0;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        out_src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        out_dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        out_src_access = 0;
        out_dst_access = VK_ACCESS_TRANSFER_WRITE_BIT;
        return;
    }

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        out_src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        out_dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        out_src_access = 0;
        out_dst_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        return;
    }

    if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        out_src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        out_src_access = VK_ACCESS_TRANSFER_WRITE_BIT;
        if (queue_flags & VK_QUEUE_GRAPHICS_BIT) {
            out_dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            out_dst_access = VK_ACCESS_SHADER_READ_BIT;
        } else if (queue_flags & VK_QUEUE_COMPUTE_BIT) {
            out_dst_stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            out_dst_access = VK_ACCESS_SHADER_READ_BIT;
        } else {
            out_dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            out_dst_access = VK_ACCESS_TRANSFER_READ_BIT;
        }
        return;
    }

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
                                                    new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ||
                                                    new_layout == VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL)) {
        out_src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        out_dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        out_src_access = 0;
        out_dst_access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        return;
    }

    if (old_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        out_src_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        out_dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        out_src_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        out_dst_access = VK_ACCESS_SHADER_READ_BIT;
        return;
    }

    if (new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && (old_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
                                                                   old_layout == VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL ||
                                                                   old_layout == VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL)) {
        out_src_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        out_dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        out_src_access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        out_dst_access = VK_ACCESS_SHADER_READ_BIT;
        return;
    }

    if (new_layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        if (old_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            out_src_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            out_src_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            out_src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            out_src_access = VK_ACCESS_TRANSFER_WRITE_BIT;
        } else {
            out_src_stage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            out_src_access = VK_ACCESS_MEMORY_WRITE_BIT;
        }

        out_dst_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        out_dst_access = 0;
        return;
    }

    out_src_stage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    out_dst_stage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    out_src_access = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    out_dst_access = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
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

void VulkanCommandBuffer::bind_graphics_pipeline(RHIGraphicsPipeline* pipeline) {
    VulkanGraphicsPipeline* vulkan_graphics_pipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
    VulkanAPI::lvkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_graphics_pipeline->get_handle());
}

void VulkanCommandBuffer::set_shader_constants(RHIGraphicsPipeline* pipeline, const RHIShaderConstants& push_constants) {
    VulkanGraphicsPipeline* vulkan_graphics_pipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
    VkShaderStageFlags stage_flags = vulkan_shader_stage_get(push_constants.stage);
    VulkanAPI::lvkCmdPushConstants(command_buffer_,
                                   vulkan_graphics_pipeline->get_layout(),
                                   stage_flags,
                                   push_constants.offset,
                                   push_constants.size,
                                   push_constants.data);
}

void VulkanCommandBuffer::bind_shader_resource_group(RHIGraphicsPipeline* pipeline, const Array<RHIShaderResourceGroup*>& groups, size_t group_index) {
    static constexpr auto mapping_group = [](RHIShaderResourceGroup*& descriptor_set) -> VkDescriptorSet {
        VulkanShaderResourceGroup* vk_descriptor_set = static_cast<VulkanShaderResourceGroup*>(descriptor_set);
        return vk_descriptor_set->get_handle();
    };

    Array<VkDescriptorSet> vk_descriptor_sets = groups.map<VkDescriptorSet>(mapping_group);
    VulkanGraphicsPipeline* vk_pipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);

    VulkanAPI::lvkCmdBindDescriptorSets(command_buffer_,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        vk_pipeline->get_layout(),
                                        group_index,
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

    VulkanAPI::lvkCmdBindVertexBuffers(command_buffer_,
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
        // TODO: Make the index type configurable
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

    VulkanAPI::lvkCmdSetScissor(command_buffer_,
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

    VulkanAPI::lvkCmdSetViewport(command_buffer_,
                                 0,
                                 count,
                                 vk_viewports.data());
}

void VulkanCommandBuffer::texture_generate_mipmap(const RHITextureLayoutTransition& transition) {
    VulkanTexture* texture = static_cast<VulkanTexture*>(transition.texture);
    VkFormat format = vulkan_format_get(texture->get_description().format);

    VulkanContext& context = vulkan_context_get();
    uint32 mip_levels = texture->get_description().mip_levels;

    VkFormatProperties format_properties;
    VulkanAPI::lvkGetPhysicalDeviceFormatProperties(context.physical_device, format, &format_properties);

    if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        LLOG_ERROR("[Vulkan]", "Texture format does not support linear blitting.");
        return;
    }

    RHITextureLayoutTransition initial_transition = transition;
    initial_transition.new_layout = RHITextureLayout::TransferDst;
    transition_texture_layout(initial_transition);

    int32 mip_width = static_cast<int32>(texture->get_description().width);
    int32 mip_height = static_cast<int32>(texture->get_description().height);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = texture->get_handle();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    for (uint32 i = 1; i < mip_levels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        VulkanAPI::lvkCmdPipelineBarrier(command_buffer_,
                                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                                         0,
                                         0, nullptr,
                                         0, nullptr,
                                         1,
                                         &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mip_width, mip_height, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1};
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        VulkanAPI::lvkCmdBlitImage(command_buffer_,
                                   texture->get_handle(),
                                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                   texture->get_handle(),
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   1,
                                   &blit,
                                   VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        VulkanAPI::lvkCmdPipelineBarrier(command_buffer_,
                                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                         0, 0, nullptr, 0, nullptr, 1,
                                         &barrier);

        if (mip_width > 1) {
            mip_width /= 2;
        }
        if (mip_height > 1) {
            mip_height /= 2;
        }
    }

    barrier.subresourceRange.baseMipLevel = mip_levels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    VulkanAPI::lvkCmdPipelineBarrier(command_buffer_,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                     0, 0, nullptr, 0, nullptr, 1,
                                     &barrier);

    if (transition.new_layout != RHITextureLayout::ShaderReadOnly) {
        RHITextureLayoutTransition final_transition = transition;
        final_transition.old_layout = RHITextureLayout::ShaderReadOnly;
        transition_texture_layout(final_transition);
    }
}

void VulkanCommandBuffer::transition_texture_layout(const RHITextureLayoutTransition& transition) {
    VulkanTexture* texture = static_cast<VulkanTexture*>(transition.texture);
    VkImageAspectFlags aspect_mask = vulkan_format_to_image_aspect(texture->get_description().format);

    VkImageMemoryBarrier image_barrier = {};
    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_barrier.oldLayout = vulkan_texture_layout_get(transition.old_layout);
    image_barrier.newLayout = vulkan_texture_layout_get(transition.new_layout);
    image_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.image = texture->get_handle();
    image_barrier.subresourceRange.aspectMask = aspect_mask;
    image_barrier.subresourceRange.baseMipLevel = 0;
    image_barrier.subresourceRange.levelCount = texture->get_description().mip_levels;
    image_barrier.subresourceRange.baseArrayLayer = 0;
    image_barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage = 0;
    VkPipelineStageFlags destination_stage = 0;
    VkAccessFlags source_access = 0;
    VkAccessFlags destination_access = 0;

    pick_pipeline_stages_and_access(vulkan_queue_type_get(queue_type_),
                                    image_barrier.oldLayout,
                                    image_barrier.newLayout,
                                    source_stage,
                                    destination_stage,
                                    source_access,
                                    destination_access);

    image_barrier.srcAccessMask = source_access;
    image_barrier.dstAccessMask = destination_access;

    VulkanAPI::lvkCmdPipelineBarrier(command_buffer_,
                                     source_stage,
                                     destination_stage,
                                     0,
                                     0, nullptr,
                                     0, nullptr,
                                     1, &image_barrier);
}

void VulkanCommandBuffer::copy_buffer_to_texture(const RHICopyBufferToTextureCommand& command) {
    VulkanBuffer* vulkan_source = static_cast<VulkanBuffer*>(command.source);
    VulkanTexture* vulkan_texture = static_cast<VulkanTexture*>(command.destination);

    VkImage vk_destination_image = vulkan_texture->get_handle();

    VkImageAspectFlags aspect = vulkan_format_to_image_aspect(vulkan_texture->get_description().format);

    uint32 width = vulkan_texture->get_description().width;
    uint32 height = vulkan_texture->get_description().height;
    
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    // Image Subresource
    region.imageSubresource.aspectMask = aspect;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    // Image Region
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1,
    };

    VulkanAPI::lvkCmdCopyBufferToImage(command_buffer_,
                                       vulkan_source->get_handle(),
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

    VulkanAPI::lvkCmdCopyBuffer(command_buffer_,
                                vksource->get_handle(),
                                vkdestination->get_handle(),
                                1,
                                &buffer_copy);
}

void VulkanCommandBuffer::begin_render_pass(const RHIRenderPassBeginInfo& begin_info) {
    VulkanRenderPass* vk_render_pass = static_cast<VulkanRenderPass*>(begin_info.render_pass);
    VulkanFramebuffer* vk_framebuffer = static_cast<VulkanFramebuffer*>(begin_info.framebuffer);

    const RHIRenderPassDescription& rp_desc = vk_render_pass->get_description();
    uint32 total_attachments = rp_desc.color_attachment_decriptions.size();

    if (total_attachments == 0) {
        VkRenderPassBeginInfo render_pass_begin_info = {};
        VulkanAPI::lvkCmdBeginRenderPass(command_buffer_, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
        return;
    }

    VkClearColorValue default_clear_color = {
        begin_info.color.x,
        begin_info.color.y,
        begin_info.color.z,
        begin_info.color.w,
    };

    VkClearDepthStencilValue depth_stencil_value = {};
    depth_stencil_value.depth = begin_info.depth;
    depth_stencil_value.stencil = begin_info.stencil;

    Array<VkClearValue> clear_values(total_attachments + 1);

    uint32 color_attachment_count = total_attachments;
    bool has_depth_attachment = rp_desc.deph_attachement_description.has_value();

    for (uint32 i = 0; i < color_attachment_count; ++i) {
        VkClearValue clear_color_value = {};
        clear_color_value.color = default_clear_color;
        clear_values.append(clear_color_value);
    }

    if (has_depth_attachment) {
        VkClearValue clear_depth_value = {};
        clear_depth_value.depthStencil = depth_stencil_value;

        clear_values.append(clear_depth_value);
    }

    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = vk_render_pass->get_handle();
    render_pass_begin_info.framebuffer = vk_framebuffer->get_handle();
    render_pass_begin_info.renderArea.offset = {
        static_cast<int32>(begin_info.area.x),
        static_cast<int32>(begin_info.area.y)};
    render_pass_begin_info.renderArea.extent = {
        static_cast<uint32>(begin_info.area.width),
        static_cast<uint32>(begin_info.area.height)};

    render_pass_begin_info.clearValueCount = clear_values.size();
    render_pass_begin_info.pClearValues = clear_values.data();

    VulkanAPI::lvkCmdBeginRenderPass(command_buffer_,
                                     &render_pass_begin_info,
                                     VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::end_render_pass() {
    VulkanAPI::lvkCmdEndRenderPass(command_buffer_);
}

void VulkanCommandBuffer::draw(const RHIDrawCommand& command) {
    VulkanAPI::lvkCmdDraw(command_buffer_,
                          command.vertex_count,
                          command.instance_count,
                          command.first_vertex,
                          command.first_instance);
}

void VulkanCommandBuffer::draw(const RHIDrawIndexedCommand& command) {
    VulkanAPI::lvkCmdDrawIndexed(command_buffer_,
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
    if ((queue_type_ & RHIQueueType::Transfer) == RHIQueueType::Transfer) {
        flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    }

    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.flags = flags;
    command_pool_create_info.queueFamilyIndex = queue_family_index_;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateCommandPool(context_.device,
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

    LICHT_VULKAN_CHECK(VulkanAPI::lvkAllocateCommandBuffers(context_.device,
                                                            &command_buffer_allocate_info,
                                                            command_buffers_.data()));

    for (size_t i = 0; i < count_; i++) {
        upper_command_buffers_[i] = lnew(upper_command_buffer_allocator_,
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
    queue_family_index_ = static_ref_cast<VulkanCommandQueue>(description.command_queue)->get_queue_family_index();
}

}  //namespace licht