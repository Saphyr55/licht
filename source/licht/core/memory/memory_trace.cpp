#include "licht/core/memory/memory_trace.hpp"

namespace licht {

void MemoryTrace::global_add_freed_bytes(uint64 size) {
    get_default().add_freed_bytes(size);
}

void MemoryTrace::global_add_allocate_bytes(uint64 size) {
    get_default().add_allocate_bytes(size);
}

usize MemoryTrace::global_get_memory_usage() {
    return get_default().get_memory_usage();
}

MemoryTrace& MemoryTrace::get_default() {
    static MemoryTrace tracer;
    return tracer;
}

void MemoryTrace::add_freed_bytes(uint64 size) {
    memory_usage_ -= size;
}

void MemoryTrace::add_allocate_bytes(uint64 size) {
    memory_usage_ += size;
}

usize MemoryTrace::get_memory_usage() const {
    return memory_usage_;
}

}  //namespace licht