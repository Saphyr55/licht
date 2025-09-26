#pragma once

#include "licht/messaging/message.hpp"

namespace licht {

class MessageHandler {
public:
    virtual void handle(MessageContext& context) {};
};

}