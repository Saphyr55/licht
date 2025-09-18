#pragma once

#include <cstdint>
#include <utility>

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"

namespace licht {

enum class MemoryAcess : uint8 {
    Read = 1 << 0,
    Write = 1 << 1,
};

enum class MemoryOwnership {
    Owner,
    NonOwner
};

enum class MemoryCategory {
    General
};

}  //namespace licht

LICHT_CORE_API void* operator new(usize size, licht::MemoryCategory category) noexcept;
LICHT_CORE_API void* operator new[](usize size, licht::MemoryCategory category) noexcept;

LICHT_CORE_API void operator delete(void* resource, licht::MemoryCategory category) noexcept;
LICHT_CORE_API void operator delete[](void* resource, licht::MemoryCategory category) noexcept;

LICHT_CORE_API void operator delete(void* resource, usize size, licht::MemoryCategory category) noexcept;
LICHT_CORE_API void operator delete[](void* resource, usize size, licht::MemoryCategory category) noexcept;

namespace licht {

class Memory {
public:
    template <typename ResourceType>
    static ResourceType* new_resource(auto... args) noexcept {
        return new (licht::MemoryCategory::General) ResourceType(std::forward<decltype(args)>(args)...);
    }

    template <typename ResourceType>
    static void delete_resource(ResourceType* resource) noexcept {
        delete resource;
    }

    /**
     * @brief Aligns the given address to the specified alignment.
     *
     * @param address The original memory address.
     * @param alignment The alignment boundary, in bytes.
     * @return The aligned memory address.
     */
    LICHT_CORE_API static uintptr_t align_address(uintptr_t address, usize alignment);

    template <typename ResourceType>
    static ResourceType* align(ResourceType* memory, usize alignment) {
        return reinterpret_cast<ResourceType*>(align_address(reinterpret_cast<uintptr_t>(memory), alignment));
    }

    /**
     * @brief Calculates the adjustment needed to align the given address.
     *
     * @param address The original memory address.
     * @param alignment The alignment boundary, in bytes.
     * @return The number of bytes needed for alignment adjustment.
     */
    LICHT_CORE_API static usize align_adjustment(const uintptr_t address, usize alignment);

    /**
     * @brief Calculates the next aligned size based on the given the memory address size and alignment.
     *
     * @param size The size of a memory address.
     * @param alignment The alignment boundary, in bytes.
     * @return The next aligned size.
     */
    LICHT_CORE_API static usize aligned_size(usize size, usize alignment);

    /**
     * @brief Checks if a memory block address is aligned to the specified alignment.
     *
     * @param block A pointer to the memory block.
     * @param alignment The alignment boundary, in bytes.
     * @return `true` if the block is aligned, `false` otherwise.
     */
    LICHT_CORE_API static bool is_aligned(const uintptr_t address, usize alignment);

    /**
     * @brief Allocates a block of memory of the specified size.
     *
     * @param size The size of the memory block to allocate, in bytes.
     * @return A pointer to the allocated memory block.
     */
    LICHT_CORE_API static void* allocate(usize size) noexcept;
    LICHT_CORE_API static void* allocate(usize size, usize alignment) noexcept;

    /**
     * @brief Frees a previously allocated block of memory.
     *
     * @param block A pointer to the memory block to free.
     * @param size The size of the memory block, in bytes.
     */
    LICHT_CORE_API static void free(void* block, usize size) noexcept;
    LICHT_CORE_API static void free(void* block, usize size, usize alignment) noexcept;

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
                                      usize size);

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
                                     usize size);

    LICHT_CORE_API static void* copy_move(void* destination,
                                          const void* source,
                                          usize size);

    LICHT_CORE_API static int compare(const void* buffer1, const void* buffer2, usize size);

public:
    Memory() = delete;
    Memory(const Memory&) = delete;
    Memory(Memory&&) = delete;
    ~Memory() = delete;

    Memory& operator=(Memory&&) = delete;
    Memory& operator=(const Memory&&) = delete;
};

}  // namespace licht
