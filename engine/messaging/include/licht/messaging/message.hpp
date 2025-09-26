#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/string/string.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

class MessageContext {
public:
    virtual void* get_raw_message() = 0;

    template<typename MessageType>
    inline MessageType* get_message() {
        return reinterpret_cast<MessageType>(get_raw_message()); 
    } 

    virtual ~MessageContext() = default;
};

}  //namespace licht
