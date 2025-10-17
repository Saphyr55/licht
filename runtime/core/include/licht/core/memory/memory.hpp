#pragma once

#include <cstdint>
#include <type_traits>

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/concepts.hpp"

#define lplacement_new(Memory) ::new (Memory)

namespace licht {

enum class MemoryAccess : uint8 {
    Read = 1 << 0,
    Write = 1 << 1,
};
LICHT_ENUM_FLAGS(MemoryAccess)

enum class MemoryOwnership {
    Owner,
    NonOwner
};

class Memory {
public:
    /**
     * @brief Aligns the given address to the specified alignment.
     *
     * @param address The original memory address.
     * @param alignment The alignment boundary, in bytes.
     * @return The aligned memory address.
     */
    LICHT_CORE_API static uintptr_t align_address(uintptr_t address, size_t alignment);

    template <typename ResourceType>
    static ResourceType* align(ResourceType* memory, size_t alignment) {
        return reinterpret_cast<ResourceType*>(align_address(reinterpret_cast<uintptr_t>(memory), alignment));
    }

    /**
     * @brief Calculates the adjustment needed to align the given address.
     *
     * @param address The original memory address.
     * @param alignment The alignment boundary, in bytes.
     * @return The number of bytes needed for alignment adjustment.
     */
    LICHT_CORE_API static size_t align_adjustment(const uintptr_t address, size_t alignment);

    /**
     * @brief Calculates the next aligned size based on the given the memory address size and alignment.
     *
     * @param size The size of a memory address.
     * @param alignment The alignment boundary, in bytes.
     * @return The next aligned size.
     */
    LICHT_CORE_API static size_t aligned_size(size_t size, size_t alignment);

    /**
     * @brief Checks if a memory block address is aligned to the specified alignment.
     *
     * @param block A pointer to the memory block.
     * @param alignment The alignment boundary, in bytes.
     * @return `true` if the block is aligned, `false` otherwise.
     */
    LICHT_CORE_API static bool is_aligned(const uintptr_t address, size_t alignment);

    /**
     * @brief Allocates a block of memory of the specified size.
     *
     * @param size The size of the memory block to allocate, in bytes.
     * @return A pointer to the allocated memory block.
     */
    LICHT_CORE_API static uint8* allocate(size_t size) noexcept;
    LICHT_CORE_API static uint8* allocate(size_t size, size_t alignment) noexcept;

    /**
     * @brief Frees a previously allocated block of memory.
     *
     * @param block A pointer to the memory block to free.
     * @param size The size of the memory block, in bytes.
     */
    LICHT_CORE_API static void free(void* block, size_t size) noexcept;
    LICHT_CORE_API static void free(void* block, size_t size, size_t alignment) noexcept;

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

    LICHT_CORE_API static void* move(void* destination,
                                     const void* source,
                                     size_t size);

    LICHT_CORE_API static int compare(const void* buffer1, const void* buffer2, size_t size);

public:
    Memory() = delete;
    Memory(const Memory&) = delete;
    Memory(Memory&&) = delete;
    ~Memory() = delete;

    Memory& operator=(Memory&&) = delete;
    Memory& operator=(const Memory&&) = delete;
};

}  // namespace licht

inline void* lalign_malloc(size_t size, size_t alignment) noexcept {
    return licht::Memory::allocate(size, alignment);
}

inline void* lmalloc(size_t size) noexcept {
    return licht::Memory::allocate(size);
}

inline void lalign_free(void* block, size_t size, size_t alignment) noexcept {
    licht::Memory::free(block, size, alignment);
}

inline void lfree(void* block, size_t size) noexcept {
    licht::Memory::free(block, size);
}

template <typename T>
inline T* lnew(licht::CAlignedAllocator auto& allocator, T&& resource) noexcept {
    void* memory = allocator.allocate(sizeof(T), alignof(T));
    if (!memory) {
        return nullptr;
    }
    lplacement_new(memory) T(std::move(resource));
    return static_cast<T*>(memory);
}

template <typename T, typename... Args>
inline T* lnew_args(licht::CAlignedAllocator auto& allocator, Args&&... args) noexcept {
    void* memory = allocator.allocate(sizeof(T), alignof(T));
    if (!memory) {
        return nullptr;
    }
    lplacement_new(memory) T(std::forward<Args>(args)...);
    return static_cast<T*>(memory);
}

template <typename T>
inline void ldelete(licht::CAlignedAllocator auto& allocator, T* ptr) noexcept {
    LCHECK(ptr);

    if constexpr (std::is_destructible_v<T>) {
        ptr->~T();
    }

    allocator.deallocate(ptr, sizeof(T), alignof(T));
}
