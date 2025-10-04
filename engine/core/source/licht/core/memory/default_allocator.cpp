
#include "licht/core/memory/default_allocator.hpp"

namespace licht {

DefaultAllocator& DefaultAllocator::get_instance() {
    static DefaultAllocator s_default_allocator;
    return s_default_allocator;
}

}  //namespace licht