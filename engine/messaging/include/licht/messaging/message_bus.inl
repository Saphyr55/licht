#pragma once

#include "message_bus.hpp"

namespace licht {

template <typename Signature>
void MessageBus::register_handler(StringRef channel, FunctionRef<Signature>&& function) {
    Callable* callable = &std::move(function);
    register_handler(channel, callable);
}

}  //namespace licht