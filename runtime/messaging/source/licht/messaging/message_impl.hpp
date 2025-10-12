#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/messaging/message.hpp"

namespace licht {

class MessageContextImpl : public MessageContext {
public:
    virtual  SharedRef<Message> get_raw_message() const override;

    virtual const Array<MessageAddress>& get_receipents() const  override;

    virtual MessageAddress get_sender() const override;

    MessageContextImpl(const Array<MessageAddress>& receipents, const MessageAddress& sender, const SharedRef<Message>& message)
        : receipents_(receipents)
        , sender_(sender)
        , message_(message) {}

private:
    Array<MessageAddress> receipents_;
    MessageAddress sender_;
    SharedRef<Message> message_;
};

}  //namespace licht