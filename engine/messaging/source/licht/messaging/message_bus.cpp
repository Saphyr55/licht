#include "licht/messaging/message_bus.hpp"
#include "licht/core/memory/shared_ref.hpp"

#include "message_impl.hpp"

namespace licht {

MessageBus::MessageBus()
    : receivers_(1024)
    , pending_messages_(1024) {
}

void MessageBus::register_receiver(const MessageAddress& address, const SharedRef<MessageReceiver>& receiver) {
    receivers_.get_or_add(address, Array<SharedRef<MessageReceiver>>()).append(receiver);
}

void MessageBus::unregister_receiver(const MessageAddress& receiver) {
    receivers_.remove(receiver);
}

void MessageBus::send(const MessageAddress& address, const SharedRef<Message>& message) {
    Array<MessageAddress> receipents;  // TODO:
    pending_messages_.append(new_ref<MessageContextImpl>(receipents, address, message));
}

void MessageBus::dispatch(const MessageAddress& address) {
    Array<SharedRef<MessageContext>> to_dispatch(1024);

    for (auto it = pending_messages_.begin(); it != pending_messages_.end();) {
        if ((*it)->get_sender() == address) {
            to_dispatch.push_back(*it);
            pending_messages_.remove(*it);
        } else {
            ++it;
        }
    }

    if (auto receipents = receivers_.get_ptr(address)) {
        for (auto& context : to_dispatch) {
            for (auto& receipent : *receipents) {
                if (receipent) {
                    receipent->receive_message(context);
                }
            }
        }
    }
}

void MessageBus::process_messages() {
    while (!pending_messages_.empty()) {
        SharedRef<MessageContext> context = pending_messages_.back();
        pending_messages_.pop();
        if (auto* it = receivers_.get_ptr(context->get_sender())) {
            for (SharedRef<MessageReceiver>& receiver : *it) {
                if (receiver) {
                    receiver->receive_message(context);
                }
            }
        }
    }
}

}  //namespace licht