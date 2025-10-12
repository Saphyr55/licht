#include "licht/core/memory/memory.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory_trace.hpp"

#include <cstdlib>
#include <cstring>

namespace licht {

uintptr_t Memory::align_address(uintptr_t address, size_t alignment) {
    LCHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
    LCHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

    return (address + (alignment - 1)) & ~(alignment - 1);
}

size_t Memory::align_adjustment(const uintptr_t address, size_t alignment) {
    LCHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
    LCHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

    const uintptr_t mask = alignment - 1;
    const uintptr_t misalignment = address & mask;
    return misalignment > 0 ? alignment - misalignment : 0;
}

size_t Memory::aligned_size(size_t size, size_t alignment) {
    return (alignment - 1) + size;
}

bool Memory::is_aligned(const uintptr_t address, size_t alignment) {
    LCHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
    LCHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

    return (address & (alignment - 1)) == 0;
}

uint8* Memory::allocate(size_t size) noexcept {
    licht::MemoryTrace::global_add_allocate_bytes(size);
    return static_cast<uint8*>(::malloc(size));
}

uint8* Memory::allocate(size_t size, size_t alignment) noexcept {
    LCHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
    LCHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

    // To ensure we can align the memory, we allocate extra bytes.
    size_t total_size = size + alignment;
    uint8* raw_memory = Memory::allocate(total_size);
    uint8* aligned_memory = align(raw_memory, alignment);

    if (aligned_memory == raw_memory) {
        // If the memory is already aligned, we need to move the pointer forward
        // to ensure we have enough space for alignment.
        aligned_memory += alignment;
    }

    // Calculate the shift for debugging purposes.
    ptrdiff_t shift = aligned_memory - raw_memory;
    LCHECK_MSG(shift >= 0 && shift <= alignment, "Alignment adjustment is out of bounds.");

    // Store the shift value just before the aligned memory for later use in deallocation.
    aligned_memory[-1] = static_cast<uint8>(shift & 0xFF);

    return aligned_memory;
}

void Memory::free(void* block, size_t size) noexcept {
    MemoryTrace::global_add_freed_bytes(size);
    ::free(block);
}

void Memory::free(void* block, size_t size, size_t alignment) noexcept {
    size_t total_size = size + alignment;
    uint8* aligned_memory = static_cast<uint8*>(block);
    // Retrieve the shift value stored just before the aligned memory.
    ptrdiff_t shift = static_cast<ptrdiff_t>(aligned_memory[-1]);
    uint8* raw_memory = aligned_memory - shift;
    free(raw_memory, total_size);
}

void* Memory::copy(void* destination,
                   const void* source,
                   size_t size) {
    LCHECK_MSG(destination, "The destination must be not a null pointer.");
    return ::memcpy(destination, source, size);
}

void* Memory::move(void* destination,
                   const void* source,
                   size_t size) {
    LCHECK_MSG(destination, "The destination must be not a null pointer.");
    return ::memmove(destination, source, size);
}

void* Memory::write(void* destination, int32 value, size_t size) {
    LCHECK_MSG(destination, "The destination must be not a null pointer.");
    return ::memset(destination, value, size);
}

int Memory::compare(const void* buffer1, const void* buffer2, size_t size) {
    return ::memcmp(buffer1, buffer2, size);
}

}  // namespace licht
