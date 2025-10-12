#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/core/string/string.hpp"
#include "message_exports.hpp"
#include "message_receiver.hpp"

namespace licht {

using MessageAddress = String;

struct LICHT_MESSAGING_API Message {
    virtual ~Message() = default;
};

class LICHT_MESSAGING_API MessageContext {
public:
    virtual SharedRef<Message> get_raw_message() const = 0;

    template <typename MessageDerivedType>
    inline SharedRef<MessageDerivedType> get_message() const {
        return static_ref_cast<MessageDerivedType>(get_raw_message());
    }

    virtual const Array<MessageAddress>& get_receipents() const = 0;

    virtual MessageAddress get_sender() const = 0;

    virtual ~MessageContext() = default;
};

}  //namespace licht
