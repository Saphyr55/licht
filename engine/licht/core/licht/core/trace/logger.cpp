#include "licht/core/trace/logger.hpp"
#include "licht/core/string/string_ref.hpp"

#include <iostream>

namespace licht {

HashMap<LogSeverity, StringRef>& Logger::get_severity_map() {

    static HashMap<LogSeverity, StringRef> s_severity_map = {
        {LogSeverity::Info, "Info"},
        {LogSeverity::Warn, "Warn"},
        {LogSeverity::Error, "Error"},
        {LogSeverity::Fatal, "Fatal"},
        {LogSeverity::Debug, "Debug"}};

    return s_severity_map;
}

Logger& Logger::get_default() {
    
    static Logger logger = [&]() {
        static ConsoleLogger console_logger;
        static CompositeLogger compiste_logger;
        compiste_logger.add_log_fn(console_logger.get_delegate());
        return Logger([&](const LogMessage& message) {
            compiste_logger.log(message);
        });
    }();

    return logger;
}

void Logger::log(const LogMessage& message) const {
    log_fn_(message);
}

LogFn Logger::get_delegate() const {
    return log_fn_;
}

void Logger::set_delegate(LogFn func) {
    log_fn_ = func;
}

Logger::Logger(LogFn fn)
    : log_fn_(fn) {}

ConsoleLogger::ConsoleLogger() {
    console_log_fn_ = [](const LogMessage& log_message) {

        const char* severity_str = Logger::get_severity_map()[log_message.severity];

        if (!severity_str) {
            severity_str = "UNKNOWN";
        }

        std::cout << "[" << severity_str << "] "
                  << "[" << log_message.channel << "]"
                  << " " << log_message.message << "\n";
    };
}

LogFn ConsoleLogger::get_delegate() {
    return console_log_fn_;
}

void CompositeLogger::add_log_fn(LogFn log_fn) {
    loggers_.append(log_fn);
}

void CompositeLogger::log(const LogMessage& message) const {
    for (const auto& logger : loggers_) {
        logger(message);
    }
}

}  // namespace licht