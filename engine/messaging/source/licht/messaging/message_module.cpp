#include "licht/messaging/message_module.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {

void MessageModule::on_load() {
    LLOG_INFO("[MessageModule]", "Message module loaded.");
}

void MessageModule::on_startup() {
    LLOG_INFO("[MessageModule]", "Message module started.");
}

void MessageModule::on_shutdown() {
    LLOG_INFO("[MessageModule]", "Message module stopped.");
}

void MessageModule::on_unload() {
    LLOG_INFO("[MessageModule]", "Message module unloaded.");
}

}