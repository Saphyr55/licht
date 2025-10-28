#include "licht/messaging/message_module.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {

LICHT_REGISTER_MODULE(MessagingModule, "licht.messaging");

void MessagingModule::on_load() {
    LLOG_INFO("[MessagingModule]", "Messaging module loaded.");
}

void MessagingModule::on_startup() {
    LLOG_INFO("[MessagingModule]", "Messaging module started.");
}

void MessagingModule::on_shutdown() {
    LLOG_INFO("[MessagingModule]", "Messaging module stopped.");
}

void MessagingModule::on_unload() {
    LLOG_INFO("[MessagingModule]", "Messaging module unloaded.");
}

}