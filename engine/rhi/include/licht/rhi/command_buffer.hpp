#pragma once

#include "licht/core/containers/array_view.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/pipeline/pipeline.hpp"
#include "licht/rhi/descriptor_set.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

class RHICommandQueue;

/**
 * @brief Base type for all RHI commands.
 */
struct RHICommand {};

/**
 * @brief Represents a draw command for rendering.
 */
struct RHIDrawCommand : RHICommand {
    uint32 vertex_count;    ///< Number of vertices to draw.
    uint32 instance_count;  ///< Number of instances to draw.
    uint32 first_vertex;    ///< Index of the first vertex.
    uint32 first_instance;  ///< Index of the first instance.
};

struct RHIDrawIndexedCommand : RHICommand {
    uint32 index_count = 0;
    uint32 instance_count = 1;
    uint32 first_index = 0;
    int32 vertex_offset = 0;
    uint32 first_instance = 0;
};

struct RHIBufferCopyCommand : RHICommand {
    size_t source_offset = 0;
    size_t destination_offset = 0;
    size_t size = 0;
};

/**
 * @brief Information required to begin a render pass.
 */
struct RHIRenderPassBeginInfo {
    RHIRenderPassHandle render_pass; ///< Handle to the render pass object.
    RHIFramebufferHandle framebuffer; ///< Target framebuffer for the render pass.
    Rect2D area; ///< Render area (typically width, height, x, y).
};

/**
 * @brief Abstract interface representing a command buffer for recording GPU commands.
 */
class RHICommandBuffer {
public:
    /**
     * @brief Begin recording commands into this command buffer.
     */
    virtual void begin(RHICommandBufferUsage usage = RHICommandBufferUsage::None) = 0;

    /**
     * @brief End recording commands into this command buffer.
     */
    virtual void end() = 0;

    /**
     * @brief Begin a render pass.
     * @param render_pass_begin_info Information describing the render pass begin.
     */
    virtual void begin_render_pass(const RHIRenderPassBeginInfo& render_pass_begin_info) = 0;

    /**
     * @brief End the current render pass.
     */
    virtual void end_render_pass() = 0;

    /**
     * @brief Bind a graphics pipeline for subsequent draw calls.
     * @param pipeline Handle to the pipeline to bind.
     */
    virtual void bind_pipeline(RHIPipelineHandle pipeline) = 0;

    virtual void bind_descriptor_sets(RHIPipelineHandle pipeline, const Array<RHIDescriptorSetRef>& descriptor_sets) = 0;

    /**
     * @brief Bind vertex buffers for rendering.
     * @param buffers Array of buffer handles to bind.
     */
    virtual void bind_vertex_buffers(const Array<RHIBufferHandle>& buffers) = 0;

    virtual void bind_index_buffer(RHIBufferHandle buffer) = 0;

    /**
     * @brief Set scissors for rendering.
     * @param scissors Pointer to an array of scissor rectangles.
     * @param count Number of scissor rectangles.
     */
    virtual void set_scissors(const Rect2D* scissors, uint32 count) = 0;

    /**
     * @brief Set viewports for rendering.
     * @param viewports Pointer to an array of viewports.
     * @param count Number of viewports.
     */
    virtual void set_viewports(const Viewport* viewports, uint32 count) = 0;

    virtual void copy_buffer(RHIBufferHandle source, RHIBufferHandle destination, const RHIBufferCopyCommand& command) = 0;

    /**
     * @brief Register a draw command.
     * @param command Draw command parameters.
     */
    virtual void draw(const RHIDrawCommand& command) = 0;

    virtual void draw(const RHIDrawIndexedCommand& command) = 0;

    /**
     * @brief Destructor.
     */
    virtual ~RHICommandBuffer() = default;
};

/**
 * @brief Shared reference type for RHICommandBuffer.
 */
using RHICommandBufferHandle = SharedRef<RHICommandBuffer>;


struct RHICommandAllocatorDescription {
    SharedRef<RHICommandQueue> command_queue;
    uint32 count = 0;
};

/**
 * @brief Abstract interface for allocating and managing command buffers.
 */
class RHICommandAllocator {
public:
    /**
     * @brief Open a command buffer for recording.
     * @param index Index identifying which buffer to open.
     * @return Handle to the opened command buffer.
     */
    virtual RHICommandBufferHandle open(uint32 index = 0) = 0;

    /**
     * @brief Reset a previously recorded command buffer for reuse.
     * @param command_buffer Handle to the command buffer to reset.
     */
    virtual void reset_command_buffer(RHICommandBufferHandle command_buffer) = 0;

    /**
     * @brief Destructor.
     */
    virtual ~RHICommandAllocator() = default;
};

/**
 * @brief Shared reference type for RHICommandAllocator.
 */
using RHICommandAllocatorRef = SharedRef<RHICommandAllocator>;

}  // namespace licht
