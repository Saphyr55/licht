#pragma once

#include <functional>
#include <utility>

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"

/**
 * @brief Overloads the global `new` operator to allocate memory.
 *
 * @param size The size of the memory block to allocate, in bytes.
 * @return A pointer to the allocated memory block.
 */
void* operator new(size_t size);

/**
 * @brief Overloads the global `new` operator to allocate memory using a custom allocator.
 *
 * @param size The size of the memory block to allocate, in bytes.
 * @param alloc A function pointer to the custom allocator.
 * @return A pointer to the allocated memory block.
 */
void* operator new(size_t size, std::function<void*(size_t size)> alloc);

/**
 * @brief Overloads the global `delete` operator to deallocate memory.
 *
 * @param resource A pointer to the memory block to deallocate.
 * @param size The size of the memory block, in bytes.
 */
void operator delete(void* resource, size_t size) noexcept;

void operator delete(void* resource, size_t size, std::function<void(void* resource, size_t size)> dealloc) noexcept;

namespace licht {

/**
 * @class Memory
 * @brief Provides memory management utilities such as allocation, deallocation, and memory manipulation.
 *
 * This class contains static methods for handling memory operations, including allocation, freeing,
 * writing values to memory, and copying memory blocks.
 */
class Memory {
public:
    Memory() = delete;
    ~Memory() = delete;

    template<typename T>
    static T* new_resource(auto... args) {
        return new T(std::forward<decltype(args)>(args)...);
    }

    template<typename T>
    static void delete_resource(T* resource) {
        if (resource) {
            delete resource;
        }
    }

    /**
     * @brief Allocates a block of memory of the specified size.
     *
     * @param size The size of the memory block to allocate, in bytes.
     * @return A pointer to the allocated memory block, or nullptr if allocation fails.
     */
    LICHT_CORE_API static void* allocate(size_t size);

    /**
     * @brief Frees a previously allocated block of memory.
     *
     * @param block A pointer to the memory block to free.
     * @param size The size of the memory block, in bytes.
     */
    LICHT_CORE_API static void free(void* block, size_t size);

    /**
     * @brief Writes a specified value into a memory block.
     *
     * @param destination A pointer to the memory block where the value will be written.
     * @param value The value to write into the memory block.
     * @param size The number of bytes to write.
     * @return A pointer to the destination memory block.
     */
    LICHT_CORE_API static void* write(void* destination,
                                      int32 value,
                                      size_t size);

    /**
     * @brief Copies a block of memory from the source to the destination.
     *
     * @param destination A pointer to the destination memory block.
     * @param source A pointer to the source memory block.
     * @param size The number of bytes to copy.
     * @return A pointer to the destination memory block.
     */
    LICHT_CORE_API static void* copy(void* destination,
                                     const void* source,
                                     size_t size);

    LICHT_CORE_API static void* copy_move(void* destination,
                                          const void* source,
                                          size_t size);

    LICHT_CORE_API static int compare(const void* buffer1, const void* buffer2, size_t size);
};

}  // namespace licht
