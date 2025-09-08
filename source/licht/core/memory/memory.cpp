#include "licht/core/memory/memory.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory_trace.hpp"

#include <cstdlib>
#include <cstring>
#include <vector>

namespace licht {

void* Memory::allocate(usize size) {
    MemoryTrace::global_add_allocate_bytes(size);
    return ::malloc(size);
}

void Memory::free(void* block, usize size) {
    LCHECK_MSG(block, "Attempt to free a null pointer.");
    MemoryTrace::global_add_freed_bytes(size);
    ::free(block);
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

}  // namespace licht

void* operator new(usize size, LichtMemoryTag tag) {
    return ::licht::Memory::allocate(size);
}

void operator delete(void* resource, usize size) noexcept {
    return ::licht::Memory::free(resource, size);
}
