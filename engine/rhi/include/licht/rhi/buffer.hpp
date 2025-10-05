#pragma once

#include <cstddef>
#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi/resource.hpp"
#include "licht/rhi/rhi_exports.hpp"
#include "licht/rhi/rhi_fowards.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/texture.hpp"


namespace licht {

/**
 * @brief Describes a vertex buffer binding for the graphics pipeline.
 *
 * This structure specifies the binding index, stride, and input rate for a vertex buffer.
 */
struct RHIVertexBindingDescription {
    /**
     * @brief The binding index for the vertex buffer.
     */
    uint32 binding = 0;

    /**
     * @brief The stride (in bytes) between elements in the vertex buffer.
     */
    uint32 stride = 0;

    /**
     * @brief The input rate for the vertex data (per-vertex or per-instance).
     */
    RHIVertexInputRate input_rate = RHIVertexInputRate::Vertex;
};

/**
 * @struct RHIVertexAttributeDescription
 * @brief Describes a single vertex attribute for the RHI (Rendering Hardware Interface).
 *
 * This struct specifies the layout of a vertex attribute, including its binding index,
 * shader location, data format, and byte offset within the vertex buffer.
 */
struct RHIVertexAttributeDescription {
    /**
     * @brief The binding index of the vertex buffer.
     * Specifies which vertex buffer this attribute is sourced from.
     */
    uint32 binding = 0;

    /**
     * @brief The location of the attribute in the shader.
     * Corresponds to the input location in the vertex shader.
     */
    uint32 location = 0;

    /**
     * @brief The data format of the attribute.
     * Specifies how the attribute data is interpreted (e.g., float, int).
     */
    RHIFormat format = RHIFormat::R32Float;

    /**
     * @brief The byte offset of the attribute within the vertex buffer.
     * Indicates where the attribute data begins in the buffer.
     */
    uint32 offset = 0;
};

/**
 * @struct RHIBufferDescription
 * @brief Describes the properties of a buffer in the RHI (Rendering Hardware Interface).
 *
 * This struct contains information about the intended usage, access mode, and size of a buffer.
 */
struct RHIBufferDescription {
    /**
     * @brief Specifies the usage type of the buffer (e.g., Vertex, Index, etc.).
     */
    RHIBufferUsage usage = RHIBufferUsage::Vertex;

    /**
     * @brief
     */
    RHIBufferMemoryUsage memory_usage = RHIBufferMemoryUsage::Host;

    /**
     * @brief Defines the access mode for the buffer (e.g., Private, Read, Write).
     */

    RHIAccessMode access_mode = RHIAccessMode::Private;

    /**
     * @brief The size of the buffer in bytes.
     */
    size_t size = 0;
};

/**
 * @brief Abstract base class representing a buffer resource in the RHI.
 *
 * RHIBuffer provides an interface for buffer objects, allowing querying of usage, access mode, size,
 * and supporting operations such as binding, mapping, unmapping, and updating buffer data.
 */
class LICHT_RHI_API RHIBuffer : public RHIResource {
public:
    /**
     * @brief Gets the usage type of the buffer.
     * @return The usage type as an RHIBufferUsage enum.
     */
    virtual RHIBufferUsage get_usage() = 0;

    /**
     * @brief Gets the access mode of the buffer.
     * @return The access mode enum.
     */
    virtual RHIAccessMode get_access_mode() = 0;

    /**
     * @brief Gets the size of the buffer in bytes.
     * @return The size of the buffer.
     */
    virtual size_t get_size() = 0;

    /**
     * @brief Binds the buffer to the current context.
     */
    virtual void bind() = 0;

    /**
     * @brief Maps a region of the buffer for CPU access.
     * @param offset The offset in bytes from the start of the buffer.
     * @param size The size in bytes to map. If 0, maps the entire buffer.
     * @return Pointer to the mapped memory region.
     */
    virtual void* map(size_t offset = 0, size_t size = 0) = 0;

    /**
     * @brief Unmaps the buffer from CPU access.
     */
    virtual void unmap() = 0;

    /**
     * @brief Updates the buffer with new data.
     * @param data Pointer to the source data.
     * @param size Size of the data in bytes.
     * @param offset Offset in bytes from the start of the buffer to update.
     */
    virtual void update(const void* data, size_t size, size_t offset = 0) = 0;

    /**
     * @brief Virtual destructor.
     */
    virtual ~RHIBuffer() = default;
};

struct LICHT_RHI_API RHIStagingBufferContext {
    RHIBufferUsage usage;
    size_t size;       // Size in bytes;
    const void* data;  // Data to transfer to the device.

    RHIStagingBufferContext(RHIBufferUsage in_usage, size_t in_size, void* in_data)
        : usage(in_usage)
        , size(in_size)
        , data(in_data) {}

    template <typename T>
    RHIStagingBufferContext(RHIBufferUsage usage, const Array<T>& data)
        : usage(usage)
        , size(data.size() * sizeof(T))
        , data(data.data()) {
    }

    ~RHIStagingBufferContext() = default;
};

class LICHT_RHI_API RHIDeviceMemoryUploader {
public:
    RHIBuffer* send_buffer(const RHIStagingBufferContext& context);

    RHITexture* send_texture(const RHIStagingBufferContext& context, const RHITextureDescription& description);

    void upload();

    RHIDeviceMemoryUploader(RHIDeviceRef device, size_t capacity = 8)
        : device_(device)
        , buffer_entries_(capacity) {}

private:
    RHIBufferDescription create_staging_buffer_description(const RHIStagingBufferContext& context);
    RHIBufferDescription create_buffer_description(const RHIStagingBufferContext& context);

private:
    struct BufferEntry {
        RHIBuffer* staging;
        RHIBuffer* main;
        size_t size;
    };

    struct TextureEntry {
        RHIBuffer* staging;
        RHITexture* main;
        size_t size;
    };

    RHIDeviceRef device_;
    Array<BufferEntry> buffer_entries_;
    Array<TextureEntry> texture_entries_;
};

}  //namespace licht