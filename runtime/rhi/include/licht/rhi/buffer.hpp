#pragma once

#include "licht/rhi/resource.hpp"
#include "licht/rhi/rhi_exports.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

/**
 * @struct RHIBufferDescription
 * @brief Describes the properties of a buffer in the RHI (Rendering Hardware Interface).
 *
 * This struct contains information about the intended usage, access mode, and size of a buffer.
 */
struct RHIBufferDescription {
    size_t size = 0;
    RHIBufferUsageFlags usage = RHIBufferUsageFlags::Vertex;
    RHIMemoryUsage memory_usage = RHIMemoryUsage::Host;
    RHISharingMode sharing_mode = RHISharingMode::Private;

    RHIBufferDescription() = default;
    RHIBufferDescription(size_t in_size,
                         RHIBufferUsageFlags in_usage = RHIBufferUsageFlags::Vertex,
                         RHIMemoryUsage in_memory_usage = RHIMemoryUsage::Host,
                         RHISharingMode in_sharing_mode = RHISharingMode::Private)
        : size(in_size)
        , usage(in_usage)
        , memory_usage(in_memory_usage)
        , sharing_mode(in_sharing_mode) {}
};

/**
 * @brief Interface representing a buffer resource in the RHI.
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
    virtual RHIBufferUsageFlags get_usage() = 0;

    /**
     * @brief Gets the sharing mode of the buffer.
     * @return The access mode enum.
     */
    virtual RHISharingMode get_sharing_mode() = 0;

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


}  //namespace licht