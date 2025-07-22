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
void* operator new(usize p_size);

/**
 * @brief Overloads the global `new` operator to allocate memory using a custom allocator.
 *
 * @param size The size of the memory block to allocate, in bytes.
 * @param alloc A function pointer to the custom allocator.
 * @return A pointer to the allocated memory block.
 */
void* operator new(usize p_size, std::function<void*(usize size)> p_alloc);

/**
 * @brief Overloads the global `delete` operator to deallocate memory.
 *
 * @param resource A pointer to the memory block to deallocate.
 * @param size The size of the memory block, in bytes.
 */
void operator delete(void* p_resource, usize p_size) noexcept;

void operator delete(void* p_resource, usize p_size, std::function<void(void* resource, usize size)> p_dealloc) noexcept;

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
    static T* new_resource(auto... p_args) {
        return new T(std::forward<decltype(p_args)>(p_args)...);
    }

    template<typename T>
    static void delete_resource(T* p_resource) {
        if (p_resource) {
            delete p_resource;
        }
    }

    /**
     * @brief Allocates a block of memory of the specified size.
     *
     * @param size The size of the memory block to allocate, in bytes.
     * @return A pointer to the allocated memory block, or nullptr if allocation fails.
     */
    LICHT_CORE_API static void* allocate(usize p_size);

    /**
     * @brief Frees a previously allocated block of memory.
     *
     * @param block A pointer to the memory block to free.
     * @param size The size of the memory block, in bytes.
     */
    LICHT_CORE_API static void free(void* p_block, usize p_size);

    /**
     * @brief Writes a specified value into a memory block.
     *
     * @param destination A pointer to the memory block where the value will be written.
     * @param value The value to write into the memory block.
     * @param size The number of bytes to write.
     * @return A pointer to the destination memory block.
     */
    LICHT_CORE_API static void* write(void* p_destination,
                                      int32 p_value,
                                      usize p_size);

    /**
     * @brief Copies a block of memory from the source to the destination.
     *
     * @param destination A pointer to the destination memory block.
     * @param source A pointer to the source memory block.
     * @param size The number of bytes to copy.
     * @return A pointer to the destination memory block.
     */
    LICHT_CORE_API static void* copy(void* p_destination,
                                     const void* p_source,
                                     usize p_size);

    LICHT_CORE_API static void* copy_move(void* p_destination,
                                          const void* p_source,
                                          usize p_size);

    LICHT_CORE_API static int compare(const void* p_buffer1, const void* p_buffer2, usize p_size);
};

}  // namespace licht
