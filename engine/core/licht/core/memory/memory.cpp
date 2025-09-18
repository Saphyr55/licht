#include "licht/core/memory/memory.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory_trace.hpp"
#include "memory.hpp"

#include <cstdlib>
#include <cstring>

namespace licht {

uintptr_t Memory::align_address(uintptr_t address, usize alignment) {
    LCHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
    LCHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

    return (address + (alignment - 1)) & ~(alignment - 1);
}

usize Memory::align_adjustment(const uintptr_t address, usize alignment) {
    LCHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
    LCHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

    const uintptr_t mask = alignment - 1;
    const uintptr_t misalignment = address & mask;
    return misalignment > 0 ? alignment - misalignment : 0;
}

usize Memory::aligned_size(usize size, usize alignment) {
    return (alignment - 1) + size;
}

bool Memory::is_aligned(const uintptr_t address, usize alignment) {
    LCHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
    LCHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

    return (address & (alignment - 1)) == 0;
}

void* Memory::allocate(usize size) noexcept {
    return static_cast<void*>(new (licht::MemoryCategory::General) uint8[size]);
}

void* Memory::allocate(usize size, usize alignment) noexcept {
    LCHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
    LCHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

    // To ensure we can align the memory, we allocate extra bytes.
    usize total_size = size + alignment;
    uint8* raw_memory = new (licht::MemoryCategory::General) uint8[total_size];
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

void Memory::free(void* block, usize size) noexcept {
    delete [] static_cast<uint8*>(block);
}

void Memory::free(void* block, usize size, usize alignment) noexcept {    
    uint8* aligned_memory = static_cast<uint8*>(block);
    // Retrieve the shift value stored just before the aligned memory.
    ptrdiff_t shift = static_cast<ptrdiff_t>(aligned_memory[-1]);
    uint8* raw_memory = aligned_memory - shift;
    delete [] raw_memory;
}

void* Memory::copy(void* destination,
                   const void* source,
                   usize size) {
    LCHECK_MSG(destination, "The destination must be not a null pointer.");
    return memcpy(destination, source, size);
}

void* Memory::copy_move(void* destination,
                        const void* source,
                        usize size) {
    LCHECK_MSG(destination, "The destination must be not a null pointer.");
    return memmove(destination, source, size);
}

void* Memory::write(void* destination, int32 value, usize size) {
    LCHECK_MSG(destination, "The destination must be not a null pointer.");
    return memset(destination, value, size);
}

int Memory::compare(const void* buffer1, const void* buffer2, usize size) {
    return memcmp(buffer1, buffer2, size);
}

} // namespace licht

void* operator new(usize size, licht::MemoryCategory category) noexcept {
    licht::MemoryTrace::global_add_allocate_bytes(size);
    return ::malloc(size);
}

void* operator new[](usize size, licht::MemoryCategory category) noexcept {
    licht::MemoryTrace::global_add_allocate_bytes(size);
    return ::malloc(size);
}

void operator delete(void* resource, licht::MemoryCategory category) noexcept {
    LCHECK_MSG(false, "Use sized delete operator to free memory allocated with category new.");
}

void operator delete[](void* resource, licht::MemoryCategory category) noexcept {
    LCHECK_MSG(false, "Use sized delete operator to free memory allocated with category new.");
}

void operator delete(void* resource, usize size, licht::MemoryCategory category) noexcept {
    LCHECK_MSG(resource, "Deleting a null pointer is not allowed.");

    licht::MemoryTrace::global_add_freed_bytes(size);
    return ::free(resource);
}

void operator delete[](void* resource, usize size, licht::MemoryCategory category) noexcept {
    LCHECK_MSG(resource, "Deleting a null pointer is not allowed.");

    licht::MemoryTrace::global_add_freed_bytes(size);
    return ::free(resource);
}
