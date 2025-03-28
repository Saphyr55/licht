#include "core/memory/memory.hpp"
#include "core/defines.hpp"
#include "core/memory/memory_trace.hpp"

#include <cstdlib>
#include <cstring>

namespace licht {

void* Memory::allocate(size_t size) {
    MemoryTrace::global_add_allocate_bytes(size);
    return ::malloc(size);
}

void Memory::free(void* block, size_t size) {
    LCHECK_MSG(block, "Attempt to free a null pointer.");
    MemoryTrace::global_add_freed_bytes(size);
    ::free(block);
}

void* Memory::copy(void* destination,
                   const void* source,
                   size_t size) {
    LCHECK(destination && source)
    return memcpy(destination, source, size);
}

void* Memory::copy_move(void* destination,
                   const void* source,
                   size_t size) {
    LCHECK(destination && source)
    return memmove(destination, source, size);
}

void* Memory::write(void* destination, int32 value, size_t size) {
    return memset(destination, value, size);
}

int Memory::compare(const void* buffer1, const void* buffer2, size_t size) {
    return memcmp(buffer1, buffer2, size);
}

}  // namespace licht

namespace pf = ::licht;

void* operator new(size_t size) {
    return ::pf::Memory::allocate(size);
}

void* operator new(size_t size, void* (*alloc)(size_t size)) {
    return alloc(size);
}

void operator delete(void* resource, size_t size) noexcept {
    return ::pf::Memory::free(resource, size);
}

void operator delete(void* resource, size_t size, void* (*dealloc)(void* resource, size_t size)) noexcept {
    dealloc(resource, size);
}

