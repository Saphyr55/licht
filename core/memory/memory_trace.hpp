#pragma once

#include "core/core_exports.hpp"
#include "core/defines.hpp"

namespace licht {

class LICHT_CORE_API MemoryTrace {
public:
    static void global_add_freed_bytes(uint64 size);

    static void global_add_allocate_bytes(uint64 size);

    static size_t global_get_memory_usage();

private:
    static MemoryTrace& get_default();

    void add_freed_bytes(uint64 size);
    void add_allocate_bytes(uint64 size);
    size_t get_memory_usage() const;

private:
    uint64 memory_usage_;
};



}  //namespace licht