#pragma once

#include "licht/core/containers/hash_map.hpp"
#include "licht/core/function/callable.hpp"
#include "licht/core/function/function_ref.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

class MessageBus {
public:
    static MessageBus& get_instance();
    
    template<typename Signature>
    void register_handler(StringRef channel, FunctionRef<Signature>&& function);

    void register_handler(StringRef channel, Callable* callable);

private:
    HashMap<StringRef, Callable*> channel_registry_; 
};

}

#include "message_bus.inl"
