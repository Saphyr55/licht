#include "message_impl.hpp"

namespace licht {

 SharedRef<Message> MessageContextImpl::get_raw_message() const {
    return message_;
}

const Array<MessageAddress>& MessageContextImpl::get_receipents() const  {
    return receipents_;
}

MessageAddress MessageContextImpl::get_sender() const  {
    return sender_;
}

}  //namespace licht