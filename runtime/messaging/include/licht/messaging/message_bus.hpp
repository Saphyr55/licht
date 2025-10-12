#pragma once

#include "licht/core/containers/hash_map.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/messaging/message.hpp"
#include "licht/messaging/message_receiver.hpp"
#include "licht/messaging/message_exports.hpp"

namespace licht {

class LICHT_MESSAGING_API MessageBus {
public:
    static MessageBus& get_instance();

    void register_receiver(const MessageAddress& address, const SharedRef<MessageReceiver>& receiver);

    void unregister_receiver(const MessageAddress& address);

    void send(const MessageAddress& address, const SharedRef<Message>& message);

    void dispatch(const MessageAddress& address);

    void process_messages();

    MessageBus();
    ~MessageBus() = default;
    
private:
    HashMap<MessageAddress, Array<SharedRef<MessageReceiver>>> receivers_;
    Array<SharedRef<MessageContext>> pending_messages_; 
};

}
