#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "message_exports.hpp"

namespace licht {

class MessageContext;

class LICHT_MESSAGING_API MessageReceiver {
public:
    virtual void receive_message(const SharedRef<MessageContext>& context) = 0; 

    virtual ~MessageReceiver() = default;
};

}